#include "file.h"

#define LOG_FILE_NAME "log.csv"
#define CONFIG_FILE_NAME "config.csv"

File logFile;
File configFile;

bool logFileOpen = false;
bool configFileOpen = false;

bool initSD()
{
	return SD.begin(SD_CS);
}

void initLogFile()
{
	if(configFileOpen)
	{
		configFile.close();
		configFileOpen = false;
	}
	if(logFileOpen)
	{
		return;
	}
	if (SD.exists(LOG_FILE_NAME))
	{
		SD.remove(LOG_FILE_NAME);
	}
	logFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
	logFile.println(LOG_TITLE);
	logFileOpen = true;
}

void readConfig(int* buf, uchr size)
{
	if(logFileOpen)
	{
		// 暂时关闭
		logFile.close();
	}
	configFile = SD.open(CONFIG_FILE_NAME, FILE_READ);
	configFileOpen = true;
	for(int i = 0;i < size && logFile.available();++i)
	{
		String tmp;
		char t;
		while((t = logFile.read()) != ',')
		{
			tmp += t;
		}
		if(tmp.length() == 0)
		{
			// 没有读到数据，以INT_MAX标记
			buf[i] = INT_MAX;
		}
		else
		{
			buf[i] = tmp.toInt();
		}
	}
	configFile.close();
	configFileOpen = false;
	if (logFileOpen)
	{
		// 恢复文件
		logFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
	}
}

void writeConfig(int* buf, uchr size)
{
	String config;
	if (logFileOpen)
	{
		// 暂时关闭
		logFile.close();
	}
	configFile = SD.open(CONFIG_FILE_NAME, FILE_WRITE);
	configFileOpen = true;
	for(int i = 0;i < size;++i)
	{
		config += String(buf[i]) + ',';
	}
	configFile.println(config);
	configFileOpen = false;
	if (logFileOpen)
	{
		// 恢复文件
		logFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
	}
}

void printLog(const int* ctrlSignals, const Point<float>& acc,const Point<float>& gyro,
			  const double& roll,const double& yaw,const double& pitch)
{
	String log;
	log += String(millis()) + ',';
	for (auto i = 1; i <= 6; ++i)
	{
		log += String(ctrlSignals[i]) + ',';
	}
	log += String(acc.x) + ',' + acc.y + ',' + acc.z + ',' +
		gyro.x + ',' + gyro.y + ',' + gyro.z + ',' +
		roll + ',' + yaw + ',' + pitch + ',';
	logFile.println(log);
}
