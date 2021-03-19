#include "FlatFileAttachment.h"

#include <iterator>

namespace ureport
{
namespace details
{
    std::vector<std::string> GetShortPreview(std::istream& stream, size_t width, size_t length)
    {
        const char delim = '\n';
        std::vector<std::string> preview;
        std::vector<char> line;
        line.resize(width);
        stream.unsetf(std::ios_base::skipws);
        std::istream_iterator<char> i(stream);
        const std::istream_iterator<char> end;

        size_t linesCount = 0;
        for (; i != end && linesCount < length; ++linesCount)
        {
            size_t c = 0;
            for (; i != end && *i != delim && c < width; ++i, ++c)
                line[c] = *i;
            preview.push_back(std::string(&line[0], c));
            for (; i != end && *i != delim; ++i)
                ;
            if (i != end)
                ++i;
        }
        return preview;
    }
}
}
