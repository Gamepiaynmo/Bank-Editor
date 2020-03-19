#include "stdafx.h"

QUuid g_zeroUuid;

BankFile::BankFile() {
	ERRCHECK(FMOD::System_Create(&m_lowsys));
	ERRCHECK(m_lowsys->init(16, FMOD_INIT_NORMAL, nullptr));
}

BankFile::~BankFile() {
	for (BankSound* subsound : m_subsounds)
		delete subsound;

	if (m_readsound) ERRCHECK(m_readsound->release());
	if (m_playsound) ERRCHECK(m_playsound->release());
	ERRCHECK(m_lowsys->close());
	ERRCHECK(m_lowsys->release());
}

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line) {
	if (result != FMOD_OK) {
		throw QString("%1(%2): FMOD error %3 - %4").arg(file).arg(line).arg(result).arg(FMOD_ErrorString(result));
	}
}

void ERRCHECK_fn(FSBANK_RESULT result, const char *file, int line) {
	if (result != FSBANK_OK && result != FSBANK_ERR_INITIALIZED) {
		throw QString("%1(%2): FSBANK error %3 - %4").arg(file).arg(line).arg(result).arg(FSBank_ErrorString(result));
	}
}

void BankFile::importFromFile(QFile &file) {
	if (!file.open(QIODevice::ReadOnly))
		throw QString("打开文件失败");
	QByteArray fdata = file.readAll();
	file.close();

	bool flag = false;
	for (char* p = fdata.data(); p <= fdata.data() + fdata.size() - 4; p++) {
		if (!memcmp(p, "FSB5", 4)) {
			int headersize = p - fdata.data();
			m_bankheader = fdata.mid(0, headersize);
			m_fsbobj = fdata.mid(headersize, fdata.size() - headersize);
			flag = true;
			break;
		}
	}

	int majorv = 0, minorv = 0;
	char* data = m_bankheader.data();
	for (int i = 0; i < m_bankheader.size(); i++) {
		if (!memcmp(data + i, "FMT ", 4)) {
			i += 4;
			i += 4; majorv = *reinterpret_cast<int*>(data + i);
			i += 4; minorv = *reinterpret_cast<int*>(data + i);
			break;
		}
	}

	if (majorv == 0x3E && minorv == 0x3B)
		m_version = 0;
	else if (majorv == 0x77 && minorv == 0x77)
		m_version = 1;
	else throw QString("不支持的Bank版本");
	if (!flag) throw QString("不可修改的bank或文件数据损坏");

	FMOD_CREATESOUNDEXINFO csex_info = { 0 };
	csex_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	csex_info.length = m_fsbobj.size();
	ERRCHECK(m_lowsys->createSound(m_fsbobj.data(), FMOD_ACCURATETIME | FMOD_CREATESTREAM | FMOD_OPENMEMORY, &csex_info, &m_readsound));
	ERRCHECK(m_lowsys->createSound(m_fsbobj.data(), FMOD_OPENMEMORY, &csex_info, &m_playsound));
	ERRCHECK(m_readsound->getFormat(&m_type, &m_format, &m_channels, &m_bits));
	ERRCHECK(m_readsound->getDefaults(&m_frequency, nullptr));

	int subsoundcnt = 0;
	ERRCHECK(m_readsound->getNumSubSounds(&subsoundcnt));
	for (int i = 0; i < subsoundcnt; i++) {
		FMOD::Sound *subreadsound = nullptr;
		ERRCHECK(m_readsound->getSubSound(i, &subreadsound));
		m_subsounds.append(new BankSound(i, subreadsound, m_channels));
	}

	std::sort(m_subsounds.begin(), m_subsounds.end(), [](const BankSound* A, const BankSound* B) {
		return A->getName() < B->getName();
	});
}

void BankFile::exportToFile(QFile &file, int quality) {
	QMap<int, QString> replacefiles;
	for (BankSound* sound : m_subsounds)
		if (!sound->m_replace.isEmpty())
			replacefiles.insert(sound->m_index, sound->m_replace);

	ERRCHECK(FSBank_Init(FSBANK_FSBVERSION_FSB5, FSBANK_INIT_NORMAL | FSBANK_INIT_GENERATEPROGRESSITEMS,
		2, QDir::temp().absolutePath().toUtf8().data()));

	FSBANK_SUBSOUND *subsoundinfo = new FSBANK_SUBSOUND[m_subsounds.size()];
	QByteArray *filepaths = new QByteArray[m_subsounds.size()];

	QTemporaryDir tempDir;
	if (!tempDir.isValid())
		throw QString("创建临时文件夹失败");

	QProgressDialog prog(nullptr);
	prog.setWindowTitle("正在生成");
	prog.setLabelText("生成临时文件");
	prog.setFixedWidth(300);
	prog.setRange(0, m_subsounds.size() * 1.5 + 16);
	prog.setValue(0);
	prog.show();
	QCoreApplication::processEvents();

	for (int i = 0; i < m_subsounds.size(); i++) {
		BankSound* sound = m_subsounds[i];
		FSBANK_SUBSOUND &ssinfo = subsoundinfo[sound->m_index];

		QByteArray &path = filepaths[sound->m_index];
		path = QDir(tempDir.path()).absoluteFilePath(sound->m_name + ".wav").toUtf8();
		if (!sound->m_replace.isEmpty()) {
			QFile::copy(sound->m_replace, path);
		} else {
			exportSound(QDir(tempDir.path()), i);
		}

		ssinfo.fileNames = new char* (path.data());
		ssinfo.numFileNames = 1;
		ssinfo.speakerMap = FSBANK_SPEAKERMAP_DEFAULT;
		ssinfo.overrideFlags = FSBANK_BUILD_DEFAULT;
		ssinfo.overrideQuality = 0;
		ssinfo.desiredSampleRate = 0;
		ssinfo.percentOptimizedRate = 0;
		prog.setValue(i + 1);
		QCoreApplication::processEvents();
	}

	prog.setLabelText("构建 FSB 文件");
	QCoreApplication::processEvents();
	QString output = QDir(tempDir.path()).absoluteFilePath("output.fsb");
	ERRCHECK(FSBank_Build(subsoundinfo, m_subsounds.size(), FSBANK_FORMAT_VORBIS,
		FSBANK_BUILD_DEFAULT | FSBANK_BUILD_DONTLOOP, quality, nullptr, output.toUtf8().data()));
	ERRCHECK(FSBank_Release());
	prog.setValue(m_subsounds.size() * 1.5);
	QCoreApplication::processEvents();

	delete[] subsoundinfo;
	delete[] filepaths;

	QFile outfsb(output);
	if (!outfsb.open(QIODevice::ReadOnly))
		throw QString("打开输出文件失败");
	QByteArray newfsbobj = outfsb.readAll();
	outfsb.close();

	prog.setLabelText("构建文件头");
	QCoreApplication::processEvents();

	QByteArray newbankheader = m_bankheader;
	for (int p = 0; p + 4 <= newbankheader.size(); p++) {
		uint &emsize = *reinterpret_cast<uint*>(newbankheader.data() + p);
		if (emsize == newbankheader.size() - p - 4 + m_fsbobj.size())
			emsize += newfsbobj.size() - m_fsbobj.size();
	}

	QList<WAVItem> wavs;
	QMap<QUuid, WAIItem> waim;
	QSet<QUuid> wais;
	QMap<QUuid, MUIItem> muim;
	QSet<QUuid> muis;
	QMap<QUuid, TLNItem> tlnm;
	for (int p = 0; p + 4 <= newbankheader.size(); p++) {
		char *data = newbankheader.data() + p;
		if (!memcmp(data, "WAV ", 4))
			wavs.append(WAVItem(data));
		else if (!memcmp(data, "WAIB", 4)) {
			WAIItem wai(data, m_version);
			waim.insert(wai.m_pWAV, wai);
			wais.insert(wai.m_guid);
		} else if (!memcmp(data, "SNDH", 4)) {
			*reinterpret_cast<uint*>(data + 0x10) = newfsbobj.size();
		}
	}

	prog.setValue(m_subsounds.size() * 1.5 + 4);
	QCoreApplication::processEvents();

	QUuid pMUI;
	for (int p = 0; p + 4 <= newbankheader.size(); p++) {
		char* data = newbankheader.data() + p;
		if (!memcmp(data, "MUIB", 4)) {
			pMUI = *reinterpret_cast<QUuid*>(data + 8);
			muis.insert(pMUI);
		}
		else if (!memcmp(data, "PLST", 4)) {
			int len = *reinterpret_cast<int*>(data + 4);
			p += 8;
			while (len-- >= 16) {
				QUuid uuid = *reinterpret_cast<GUID*>(newbankheader.data() + p);
				if (wais.find(uuid) != wais.end())
					muim.insert(uuid, MUIItem(pMUI, uuid));
				p++;
			}
		}
	}

	prog.setValue(m_subsounds.size() * 1.5 + 8);
	QCoreApplication::processEvents();

	for (int p = 0; p + 4 <= newbankheader.size(); p++) {
		char* data = newbankheader.data() + p;
		if (!memcmp(data, "TLNB", 4)) {
			int len = *reinterpret_cast<int*>(data + 4);
			p += 8;
			while (len-- >= 24) {
				QUuid uuid = *reinterpret_cast<GUID*>(newbankheader.data() + p);
				if (wais.find(uuid) != wais.end() || muis.find(uuid) != muis.end())
					tlnm.insert(uuid, TLNItem(newbankheader.data() + p));
				p++;
			}
		}
	}

	prog.setValue(m_subsounds.size() * 1.5 + 12);
	QCoreApplication::processEvents();

	for (int i = 0; i < m_subsounds.size(); i++) {
		BankSound* sound = m_subsounds[i];
		if (!sound->m_replace.isEmpty()) {
			WAVItem &wav = wavs[sound->m_index];
			auto iwai = waim.find(wav.m_guid);
			if (iwai != waim.end()) {
				auto itln = tlnm.find(iwai->m_guid);
				if (itln != tlnm.end()) {
					uint new_length = sound->m_newlength / sound->m_newfreq * sound->m_samplerate
						* *itln->m_length / sound->m_length;
					*itln->m_length = new_length;
				}
				else {
					auto imui = muim.find(iwai->m_guid);
					if (imui != muim.end()) {
						auto itln = tlnm.find(imui->m_pMUI);
						if (itln != tlnm.end()) {
							*itln->m_length = std::max(*itln->m_length,
								(uint) (sound->m_newlength / sound->m_newfreq * sound->m_samplerate) * 2);
						}
					}
				}
			}
		}
	}

	prog.setValue(m_subsounds.size() * 1.5 + 16);
	QCoreApplication::processEvents();

	if (!file.open(QIODevice::WriteOnly))
		throw QString("打开文件失败");
	file.write(newbankheader);
	file.write(newfsbobj);
	file.close();
}

void BankFile::playSound(int index) {
	bool isplaying;
	if (m_channel != nullptr) {
		m_channel->isPlaying(&isplaying);
		if (isplaying)
			m_channel->stop();
	}

	FMOD::Sound *subplaysound = nullptr;
	ERRCHECK(m_playsound->getSubSound(m_subsounds[index]->m_index, &subplaysound));
	ERRCHECK(m_lowsys->playSound(subplaysound, nullptr, false, &m_channel));
}

void BankFile::playReplaceSound(int index) {
	bool isplaying;
	if (m_channel != nullptr) {
		m_channel->isPlaying(&isplaying);
		if (isplaying)
			m_channel->stop();
	}

	if (m_subsounds[index]->m_newsound != nullptr) {
		ERRCHECK(m_lowsys->playSound(m_subsounds[index]->m_newsound, nullptr, false, &m_channel));
	}
}

QString BankFile::exportSound(QDir &dir, int index) {
	FMOD::Sound *subreadsound = nullptr;
	ERRCHECK(m_readsound->getSubSound(m_subsounds[index]->m_index, &subreadsound));
	return m_subsounds[index]->exportSound(dir, subreadsound);
	
}

void BankFile::setReplaceFile(int index, QString path) {
	if (m_subsounds[index]->m_newsound != nullptr)
		ERRCHECK(m_subsounds[index]->m_newsound->release());

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		throw QString("打开文件失败");
	QByteArray data = file.readAll();
	file.close();

	FMOD::Sound *&repsound = m_subsounds[index]->m_newsound;
	FMOD_CREATESOUNDEXINFO csex_info = { 0 };
	csex_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	csex_info.length = data.size();
	ERRCHECK(m_lowsys->createSound(data.data(), FMOD_OPENMEMORY, &csex_info, &repsound));
	ERRCHECK(repsound->getLength(&m_subsounds[index]->m_newlength, FMOD_TIMEUNIT_PCM));
	ERRCHECK(repsound->getDefaults(&m_subsounds[index]->m_newfreq, nullptr));
	m_subsounds[index]->m_replace = path;
}

void BankFile::clearReplaceFile(int index) {
	if (m_subsounds[index]->m_newsound != nullptr) {
		ERRCHECK(m_subsounds[index]->m_newsound->release());
		m_subsounds[index]->m_replace = "";
		m_subsounds[index]->m_newsound = nullptr;
	}
}

BankSound::BankSound(int index, FMOD::Sound *readsound, int channels) {
	char name[1024];
	ERRCHECK(readsound->getName(name, 1024));
	m_name = name;

	m_index = index;
	ERRCHECK(readsound->getLength(&m_length, FMOD_TIMEUNIT_PCM));
	ERRCHECK(readsound->getFormat(&m_type, &m_format, &m_channels, &m_bits));
	ERRCHECK(readsound->getDefaults(&m_samplerate, nullptr));
	m_channels = channels;
}

BankSound::~BankSound() {

}

QString BankSound::exportSound(QDir &dir, FMOD::Sound *sound) {
	ERRCHECK(sound->getLength(&m_length, FMOD_TIMEUNIT_PCM));
	ERRCHECK(sound->getFormat(&m_type, &m_format, nullptr, &m_bits));
	ERRCHECK(sound->getDefaults(&m_samplerate, nullptr));

	uint length = m_length * m_channels * m_bits / 8, bytesread;
	char* buffer = new char[length];
	ERRCHECK(sound->seekData(0));
	for (int i = 0; i < length; i++) {
		if (sound->readData(buffer + i, 1, &bytesread) != FMOD_OK)
			break;
	}
	ERRCHECK(sound->readData(buffer, length, &bytesread));
	QFile file(dir.absoluteFilePath(m_name + ".wav"));
	if (!file.open(QIODevice::WriteOnly))
		throw QString("打开文件失败");

#pragma pack(1)
	struct WAVE_FORMAT {
		WORD wFormatTag;
		WORD wChannels;
		DWORD dwSamplesPerSec;
		DWORD dwAvgBytesPerSec;
		WORD wBlockAlign;
		WORD wBitsPerSample;
	} wavefmt;
#pragma pack()

	file.write("RIFF", 4);
	uint riffsize = 0x24 + length;
	file.write(reinterpret_cast<char*>(&riffsize), 4);
	file.write("WAVEfmt ", 8);
	uint fmtsize = sizeof(wavefmt);
	file.write(reinterpret_cast<char*>(&fmtsize), 4);

	wavefmt.wFormatTag = WAVE_FORMAT_PCM;
	wavefmt.wChannels = m_channels;
	wavefmt.dwSamplesPerSec = m_samplerate;
	wavefmt.dwAvgBytesPerSec = wavefmt.dwSamplesPerSec * m_channels * m_bits / 8;
	wavefmt.wBlockAlign = 4;
	wavefmt.wBitsPerSample = m_bits;

	file.write(reinterpret_cast<char*>(&wavefmt), sizeof(wavefmt));
	file.write("data", 4);
	file.write(reinterpret_cast<char*>(&length), 4);
	file.write(buffer, length);
	file.close();
	delete[] buffer;

	return m_name + ".wav";
}