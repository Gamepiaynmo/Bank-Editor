#include "stdafx.h"
#include "BankEditor.h"
#include <QtWidgets/QApplication>

int passed = 0, total = 0;
QMap<int, int> old, now;

bool test_hash(char* mem) {
	int cnt = 0;
	for (int i = 0; i < 16; i++) {
		if (*(mem + i) == 0)
			cnt++;
	}
	if (cnt >= 4)
		return false;
	return true;
}

bool test_bank(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	total++;

	QByteArray filedata = file.readAll();
	char* data = filedata.data();
	int size = filedata.size();

	int majorv = 0, minorv = 0;
	int i = 0;
	for (; i < size; i++) {
		if (!memcmp(data + i, "FMT ", 4)) {
			i += 4; if (*reinterpret_cast<int*>(data + i) != 8) return false;
			i += 4; majorv = *reinterpret_cast<int*>(data + i);
			i += 4; minorv = *reinterpret_cast<int*>(data + i);
			break;
		}
	}

	int lastp = 0;
	QVector<QByteArray> hashes;
	if (majorv == 0x3E && minorv == 0x3B) {
		for (; i < size; i++) {
			if (!memcmp(data + i, "WAV ", 4)) {
				if (*(int*)(data + i + 4) != 0x1A)
					return false;
			}
			if (!memcmp(data + i, "WAIB", 4)) {
				if (*(int*)(data + i + 4) != 0x24)
					return false;
				hashes.append(QByteArray(data + i + 8, 16));
			}
			if (!memcmp(data + i, "SNDH", 4)) {
				break;
			}
		}

		for (i = 0; i < size; i++) {
			if (!memcmp(data + i, "TLNB", 4)) {
				lastp = i;
			}
			if (lastp > 0) {
				for (QByteArray& hash : hashes) {
					if (!memcmp(data + i, hash.data(), hash.size())) {
						old[i - lastp]++;
					}
				}
			}
			if (!memcmp(data + i, "WAIB", 4)) {
				break;
			}
		}
	}
	else if (majorv == 0x77 && minorv == 0x77) {
		for (; i < size; i++) {
			if (!memcmp(data + i, "WAV ", 4)) {
				if (*(int*)(data + i + 4) != 0x1E)
					return false;
			}
			if (!memcmp(data + i, "WAIB", 4)) {
				if (*(int*)(data + i + 4) != 0x20)
					return false;
				hashes.append(QByteArray(data + i + 8, 16));
			}
			if (!memcmp(data + i, "SNDH", 4)) {
				break;
			}
		}

		for (i = 0; i < size; i++) {
			if (!memcmp(data + i, "TLNB", 4)) {
				lastp = i;
			}
			if (lastp > 0) {
				for (QByteArray& hash : hashes) {
					if (!memcmp(data + i, hash.data(), hash.size())) {
						now[i - lastp]++;
					}
				}
			}
			if (!memcmp(data + i, "WAIB", 4)) {
				break;
			}
		}
	}
	else {
		return false;
	}

	passed++;
	return true;
}

void test_banks(QString dirname) {
	QDir dir(dirname);
	for (QFileInfo info : dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
		if (info.isDir())
			test_banks(info.absoluteFilePath());
		else if (!test_bank(info.absoluteFilePath()))
			qDebug() << info.absoluteFilePath();
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
#if false
	test_banks("audio");
	test_banks("audio_old");
	qDebug() << old << now;
	qDebug() << "passed:" << passed << '/' << total;
#endif
	BankEditor w;
	w.show();
	return a.exec();
}
