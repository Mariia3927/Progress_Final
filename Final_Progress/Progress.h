#pragma once
#include "stdafx.h"

const int g_blockSize = 4096;
const int g_maxPercents = 100;

class Progress;

class IProgressObserver {
public:
	virtual void OnValueChanged(Progress* obj) = 0;
};

class Progress
{
public:
	Progress(Progress* parent = nullptr);

	bool AddObserver(IProgressObserver* observer);
	bool RemoveObserver(IProgressObserver* observer);

	bool AddChild(Progress* childProgress);
	bool RemoveChild(Progress* childProgress);
	std::vector<Progress*> GetChildProgresses();

	bool SetProgressValue(int value);
	int GetProgressValue() const { return m_currentValue; }
	int GetProgressPercentValue() const { return m_currentPercentValue; }
	bool SetEndProgressValue(int value);
	int GetEndProgressValue() const { return m_endValue; }
	void Cancel(bool flag);
	bool IsCancelled() const { return m_cancel; }
	bool End() const { return m_end; }

	bool CalculateBlocksCount(const std::vector<std::string>& fileName);

private:
	void OnNotifyObservers();
	void OnNotifyParent();
	int CalculateProgress(int currentValue, int endValue);

private:
	Progress* m_parent;
	int m_currentValue = 0;
	int m_endValue = 0;
	int m_currentPercentValue = 0;
	int m_previousPercentValue = 0;
	bool m_cancel = false;
	bool m_end = false;

	std::vector<IProgressObserver*> m_observers = {};
	std::vector<Progress*> m_childProgresses = {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ProgressObserver : public IProgressObserver
{
public:
	void OnValueChanged(Progress* obj) override;

private:
	void ShowProgress(Progress* progress);

private:
	int m_currentState = 0;
	bool m_flag = true;
};


bool GetFileSize(std::ifstream& f, int& numOfBlocks, int& additionalNumOfBytes);
bool CopyFiles(const std::vector<std::string>& fileName, Progress* p);