#pragma once

#include <string>

namespace ureport
{
    struct SearchResultItem
    {
        std::string m_Title;
        std::string m_Url;
        std::string m_SourceName;

        SearchResultItem(const std::string& title, const std::string url, const std::string sourceName)
            : m_Title(title)
            , m_Url(url)
            , m_SourceName(sourceName)
        {}
    };
}
