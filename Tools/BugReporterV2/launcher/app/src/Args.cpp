#include "Args.h"

#include <set>
#include <map>
#include <algorithm>

namespace ureport
{
namespace details
{
    class ArgsImpl : public Args
    {
        std::set<std::string> m_Keywords;
        std::map<std::string, std::vector<std::string> > m_Values;
        std::vector<std::string>* m_CurrentKeywordStore;

    public:
        ArgsImpl(const std::string& scheme, const std::vector<std::string>& args)
            : m_CurrentKeywordStore(nullptr)
        {
            ParseScheme(scheme);
            ParseArgs(args);
        }

    private:
        std::vector<std::string> GetValues(const std::string& name) const
        {
            auto iter = m_Values.find(name);
            return iter != m_Values.end() ? iter->second : std::vector<std::string>();
        }

        void ParseScheme(const std::string& scheme)
        {
            for (std::string parsed = scheme, keyword; !parsed.empty();
                 m_Keywords.insert(ExtractFirstKeyword(parsed))) ;
        }

        std::string ExtractFirstKeyword(std::string& scheme) const
        {
            auto delimPosition = scheme.find(',');
            std::string keyword;
            if (delimPosition != std::string::npos)
                keyword = scheme.substr(0, delimPosition), scheme = scheme.substr(delimPosition + 1);
            else
                keyword = scheme, scheme = "";
            return keyword;
        }

        void ParseArgs(const std::vector<std::string>& args)
        {
            std::for_each(args.begin(), args.end(), [this] (const std::string& arg)
            {
                if (IsKeyword(arg))
                    SwitchCurrentKeyword(arg.substr(2));
                else
                    AddValueToCurrentKeyword(arg);
            });
        }

        bool IsKeyword(const std::string& arg)
        {
            return arg.find("--") == 0 && m_Keywords.find(arg.substr(2)) != m_Keywords.end();
        }

        void SwitchCurrentKeyword(const std::string& keyword)
        {
            auto iter = m_Values.find(keyword);
            if (iter == m_Values.end())
                iter = m_Values.insert(std::make_pair(keyword, std::vector<std::string>())).first;
            m_CurrentKeywordStore = &(iter->second);
        }

        void AddValueToCurrentKeyword(const std::string& value)
        {
            if (m_CurrentKeywordStore != nullptr)
                m_CurrentKeywordStore->push_back(value);
            m_CurrentKeywordStore = nullptr;
        }
    };
}

    std::unique_ptr<Args> Args::Parse(const std::string& scheme,
        const std::vector<std::string>& args)
    {
        return std::unique_ptr<Args>(new details::ArgsImpl(scheme, args));
    }
}
