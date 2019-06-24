#include "stdafx.h"


Progress::Progress(Progress* parent) : m_parent(parent)
{
}

bool Progress::AddObserver(IProgressObserver* observer)
{
	if (!observer)
	{
		std::cout << "Error! AddObserver func got a nullpointer." << std::endl;
		return false;
	}

	m_observers.push_back(observer);
	return true;
}

bool Progress::RemoveObserver(IProgressObserver* observer)
{
	if (!observer)
	{
		std::cout << "Error! RemoveObserver func got a nullpointer." << std::endl;
		return false;
	}

	std::remove(m_observers.begin(), m_observers.end(), observer);

	return true;
}

bool Progress::AddChild(Progress* childProgress)
{
	m_childProgresses.push_back(childProgress);

	return true;
}

bool Progress::RemoveChild(Progress* childProgress)
{
	std::remove(m_childProgresses.begin(), m_childProgresses.end(), childProgress);

	return true;
}

std::vector<Progress*> Progress::GetChildProgresses()
{
	return m_childProgresses;
}

bool Progress::SetProgressValue(int value)
{
	if (value < 0)
	{
		std::cout << "Error! The value of progress cannot be less than zero." << std::endl;
		return false;
	}

	m_currentValue = value;

	if (m_parent)
	{
		m_parent->OnNotifyParent();
	}

	return true;
}

bool Progress::SetEndProgressValue(int value)
{
	if (value < 0)
	{
		std::cout << "Error! Progress end value cannot be less than zero.";
		return false;
	}

	m_endValue = value;

	return true;
}

void Progress::Cancel(bool flag)
{
	m_cancel = flag;
}

void Progress::OnNotifyObservers()
{
	for (unsigned int i = 0; i < m_observers.size(); i++)
	{
		m_observers[i]->OnValueChanged(this);
	}
}

void Progress::OnNotifyParent()
{
	int result = 0;
	int currentValue = 0;
	for (unsigned int i = 0; i < m_childProgresses.size(); i++)
	{
		result += CalculateProgress(m_childProgresses[i]->GetProgressValue(), this->GetEndProgressValue());
		currentValue += m_childProgresses[i]->GetProgressValue();
	}
	m_previousPercentValue = m_currentPercentValue;

	m_currentValue = currentValue;
	if (m_currentValue == m_endValue)
	{
		m_currentPercentValue = g_maxPercents;
		m_end = true;
	}
	else
	{
		m_currentPercentValue = result;
	}

	if (m_previousPercentValue < m_currentPercentValue || m_end)
	{
		OnNotifyObservers();
	}
}

int Progress::CalculateProgress(int currentValue, int endValue)
{
	if (currentValue < 0 || endValue < 0)
	{
		std::cout << "Error! The progress values cannot be less than zero!" << std::endl;
		return 0;
	}

	int result = 0;

	if (endValue != 0)
	{
		result = currentValue * g_maxPercents / endValue;
	}

	return result;
}

bool Progress::CalculateBlocksCount(const std::vector<std::string>& fileName)
{
	int result = 0, numOfBlocks = 0, additionalNumOfBytes = 0;

	if (fileName.size() != m_childProgresses.size())
	{
		std::cout << "Error! The number of files does not equal the number of child progresses." << std::endl;
		return false;
	}

	for (unsigned int i = 0; i < fileName.size(); i++)
	{
		std::ifstream fin(fileName[i], std::ios::in | std::ios::binary);
		if (!fin)
		{
			std::cout << "Error! The file for copying is incorrect!" << std::endl;
			continue;
		}

		GetFileSize(fin, numOfBlocks, additionalNumOfBytes);

		if (additionalNumOfBytes > 0)
			numOfBlocks += 1;

		m_childProgresses[i]->m_endValue = numOfBlocks;
		result += numOfBlocks;
		numOfBlocks = additionalNumOfBytes = 0;

		fin.close();
	}

	m_endValue = result;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CopyFiles(const std::vector<std::string>& fileName, Progress* progress)
{
	if (!progress)
	{
		std::cout << "Error! The Progress object is incorrect." << std::endl;
		return false;
	}

	if (!progress->CalculateBlocksCount(fileName))
	{
		std::cout << "Error! CopyFiles func cannot work because the number of file names and number of child progresses does not equal." << std::endl;
		return false;
	}

	int tmp = 0;
	char buf[g_blockSize] = "";
	int numOfBlocks = 0, additionalNumOfBytes = 0;
	char fileNameForOutput[256] = " .mp3";

	std::vector<Progress*> childOfMainProgress = progress->GetChildProgresses();
	char letter = 'a';
	for (unsigned int i = 0; i < fileName.size(); i++)
	{
		std::ifstream fin(fileName[i], std::ios::in | std::ios::binary);
		if (!fin)
		{
			std::cout << "Error! The file for copying is incorrect!" << std::endl;
			return false;
		}

		GetFileSize(fin, numOfBlocks, additionalNumOfBytes);
		if (additionalNumOfBytes > 0)
			numOfBlocks += 1;

		*fileNameForOutput = letter++;
		std::fstream fout(fileNameForOutput, std::ios::out | std::ios::binary);
		if (!fout)
		{
			std::cout << "Error! The destination file is incorrect!" << std::endl;
			return false;
		}

		fin.seekg(0, std::ios::beg);
		fout.seekp(0, std::ios::beg);

		while (tmp < numOfBlocks && !progress->IsCancelled())
		{
			fin.seekg(tmp * g_blockSize, std::ios::beg);

			if (tmp == numOfBlocks - 1 && additionalNumOfBytes > 0)
			{
				fin.read(buf, additionalNumOfBytes);
				fout.write(buf, additionalNumOfBytes);
			}
			else
			{
				fin.read(buf, g_blockSize);
				fout.write(buf, g_blockSize);
			}
			tmp++;

			childOfMainProgress[i]->SetProgressValue(tmp);
		}

		fin.close();
		fout.close();

		if (progress->IsCancelled())
		{
			return true;
		}


		tmp = numOfBlocks = additionalNumOfBytes = 0;
		strcpy_s(buf, "");
	}

	return true;
}

bool GetFileSize(std::ifstream& f, int& numOfBlocks, int& additionalNumOfBytes)
{
	if (!f)
	{
		std::cout << "Error! This file wasn't opened!" << std::endl;
		return false;
	}

	f.seekg(0, std::ios::end);
	numOfBlocks = f.tellg() / g_blockSize;
	additionalNumOfBytes = f.tellg() % g_blockSize;
	f.seekg(0, std::ios::beg);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProgressObserver::OnValueChanged(Progress* progress)
{
	m_currentState = progress->GetProgressPercentValue();

	ShowProgress(progress);
}

void ProgressObserver::ShowProgress(Progress* progress)
{
	if (m_flag)
	{
		char key;
		std::cout << "Do you want to cancel the progress? If yes - press 'q', else - press any other key." << std::endl;
		std::cin >> key;
		if (key == 'q')
			progress->Cancel(true);
		m_flag = false;
	}

	std::cout << m_currentState << "% ... ";

	if (progress->IsCancelled())
	{
		std::cout << "\nOperation cancelled!" << std::endl;
	}

	if (progress->End())
	{
		std::cout << "\nEnd!" << std::endl;
	}
}
