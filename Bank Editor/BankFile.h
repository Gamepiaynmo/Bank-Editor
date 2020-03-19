#pragma once

extern void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line);
extern void ERRCHECK_fn(FSBANK_RESULT result, const char *file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

struct BankItem {
	char m_head[4];
	QUuid m_guid;

	BankItem(const char* head, QUuid guid) : m_guid(guid) {
		memcpy(m_head, head, 4);
	}

	bool checkHeader(const char* head) {
		return !memcmp(m_head, head, 4);
	}
};

struct WAVItem : public BankItem {
	WAVItem(char* data) : BankItem("WAVS", *reinterpret_cast<GUID*>(data + 8)) {}
};

struct WAIItem : public BankItem {
	QUuid m_pWAV;

	WAIItem(char* data, int version) : BankItem("WAIS", *reinterpret_cast<GUID*>(data + 8)) {
		m_pWAV = *reinterpret_cast<GUID*>(data + (version == 0 ? 0x1c : 0x18));
	}
};

struct TLNItem {
	QUuid m_pWAI;
	uint* m_length;

	TLNItem(char* data) {
		m_pWAI = *reinterpret_cast<GUID*>(data);
		m_length = reinterpret_cast<uint*>(data + 20);
	}
};

struct MUIItem {
	QUuid m_pMUI;
	QUuid m_PLST;

	MUIItem(QUuid mui, QUuid plst) {
		m_pMUI = mui;
		m_PLST = plst;
	}
};

class BankSound {
	friend class BankFile;
public:
	BankSound(int index, FMOD::Sound *readsound, int channels);
	~BankSound();

	int getIndex() const { return m_index; }
	QString getName() const { return m_name; }
	uint getLength() const { return m_length; }
	int getLengthMs() const { return m_length / (m_samplerate / 1000); }
	int getNewLengthMs() const { return m_newlength / (m_newfreq / 1000); }

	QString exportSound(QDir &dir, FMOD::Sound *sound);

private:
	FMOD_SOUND_TYPE m_type;
	FMOD_SOUND_FORMAT m_format;

	int m_channels, m_bits;
	float m_samplerate;
	
	int m_index;
	QString m_name;
	uint m_length;

	QString m_replace;
	uint m_newlength;
	float m_newfreq;
	FMOD::Sound *m_newsound = nullptr;
};

class BankFile {
	
public:
	BankFile();
	~BankFile();

	void importFromFile(QFile &file);
	void exportToFile(QFile &file, int quality);

	int getSubsoundCount() { return m_subsounds.size(); }
	BankSound* getSubsound(int index) { return m_subsounds[index]; }

	void updateFmod() { ERRCHECK(m_lowsys->update()); }
	void playSound(int index);
	void playReplaceSound(int index);

	void setReplaceFile(int index, QString path);
	void clearReplaceFile(int index);

	QString exportSound(QDir &dir, int index);

private:
	FMOD::System *m_lowsys = nullptr;
	FMOD_RESULT m_fres;
	FMOD::Channel *m_channel = nullptr;

	FMOD::Sound *m_readsound = nullptr, *m_playsound = nullptr;
	QList<BankSound*> m_subsounds;

	FMOD_SOUND_TYPE m_type;
	FMOD_SOUND_FORMAT m_format;
	int m_channels, m_bits;
	float m_frequency;
	int m_version;

	QByteArray m_bankheader;
	QByteArray m_fsbobj;
};

