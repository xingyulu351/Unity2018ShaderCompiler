#include "FeedbackCollection.h"

namespace ureport
{
    FeedbackCollection::FeedbackCollection() : m_shouldReportBePrevented(false)
    {
    }

    bool FeedbackCollection::HasCriticalIssue()
    {
        for (auto f = AllFeedback.begin(); f != AllFeedback.end(); f++)
        {
            auto feedback = f->second;
            if (feedback.severity > 1)
                return true;
        }
        return false;
    }

    bool FeedbackCollection::PreventReporting()
    {
        return m_shouldReportBePrevented;
    }

    bool FeedbackCollection::Contains(std::map<std::string, Feedback> collection, std::string key)
    {
        return collection.count(key) != 0;
    }

    void FeedbackCollection::UpdateLocalFeedback(std::map<std::string, Feedback> input)
    {
        AddToCollection(&m_LocalFeedback, input);
    }

    void FeedbackCollection::UpdateRemoteFeedback(std::map<std::string, Feedback> input)
    {
        AddToCollection(&m_RemoteFeedback, input);
    }

    void FeedbackCollection::AddToCollection(std::map<std::string, Feedback> *collection, std::map<std::string, Feedback> data)
    {
        collection->clear();
        for (auto f = data.begin(); f != data.end(); f++)
        {
            (*collection)[f->first] = f->second;
        }
        MergeFeedback();
        emit FeedbackUpdated();
    }

    void FeedbackCollection::ResetRemoteFeedback()
    {
        m_RemoteFeedback.clear();
        MergeFeedback();
    }

    void FeedbackCollection::MergeFeedback()
    {
        AllFeedback.clear();
        for (auto f = m_LocalFeedback.begin(); f != m_LocalFeedback.end(); f++)
        {
            if (!Contains(m_RemoteFeedback, f->first))
            {
                AllFeedback[f->first] = f->second;
            }
            else
            {
                if (f->second.severity > m_RemoteFeedback[f->first].severity)
                    AllFeedback[f->first] = f->second;
                else
                    AllFeedback[f->first] = m_RemoteFeedback[f->first];
            }
        }
        for (auto f = m_RemoteFeedback.begin(); f != m_RemoteFeedback.end(); f++)
        {
            if (f->second.severity == 4)
            {
                m_shouldReportBePrevented = true;
                AllFeedback.clear();
                AllFeedback[f->first] = f->second;
                return;
            }

            if (!Contains(m_LocalFeedback, f->first))
            {
                AllFeedback[f->first] = f->second;
            }
            else
            {
                if (f->second.severity >= m_LocalFeedback[f->first].severity)
                    AllFeedback[f->first] = f->second;
                else
                    AllFeedback[f->first] = m_LocalFeedback[f->first];
            }
        }
    }

    std::unique_ptr<FeedbackCollection> CreateFeedbackCollection()
    {
        return std::unique_ptr<FeedbackCollection>(new FeedbackCollection());
    }
}
