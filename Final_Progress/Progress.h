#pragma once
#include "stdafx.h"

const int g_BlockSize = 4096;
const int g_MaxPercents = 100;

class Progress;

class IObserver {
public:
	virtual void OnValueChanged(Progress* obj) = 0;
};

class Progress
{
public:
	Progress(Progress* parent = nullptr);

	bool AddObserver(IObserver* observer);
	bool RemoveObserver(IObserver* observer);

	bool AddChild(Progress* childProgress);
	bool RemoveChild(Progress* childProgress);
	std::vector<Progress*> GetChildProgresses();

	bool SetProgressValue(int value);
	int GetProgressValue() const { return m_CurrentValue; }
	int GetProgressPercentValue() const { return m_CurrentPercentValue; }
	bool SetEndProgressValue(int value);
	int GetEndProgressValue() const { return m_EndValue; }
	void Cancel(bool flag);
	bool IsCancelled() const { return m_Cancel; }
	bool End() const { return m_End; }

	bool CalculateBlocksCount(std::vector<std::string> fileName);

private:
	void OnNotifyObservers();
	void OnNotifyParent();
	int CalculateProgress(int currentValue, int endValue);

private:
	Progress* m_Parent;
	int m_CurrentValue = 0;
	int m_EndValue = 0;
	int m_CurrentPercentValue = 0;
	int m_PreviousPercentValue = 0;
	bool m_Cancel = false;
	bool m_End = false;

	std::vector<IObserver*> m_Observers = {};
	std::vector<Progress*> m_ChildProgresses = {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ProgressObserver : public IObserver
{
public:
	void OnValueChanged(Progress* obj) override;

private:
	void ShowProgress(Progress* progress);

private:
	int m_CurrentState = 0;
	bool m_Flag = true;
};


bool GetFileSize(std::ifstream& f, int& numOfBlocks, int& additionalNumOfBytes);
bool CopyFiles(std::vector<std::string> fileName, Progress* p);