#include "JsonParser.h"

namespace JsonParser
{
    String GetJsonData(const String &json, const String &name, bool &isOk)
    {
        String findName = name;
        if (findName[0] != '"')
        {
            findName = String(F("\"")) + name + String(F("\""));
        }

        auto dataStart = json.indexOf(findName);
        if (dataStart == -1)
        {
            isOk = false;
            return String();
        }

        int startInd = json.indexOf(':', dataStart);
        if (startInd == -1)
        {
            isOk = false;
            return String();
        }

        String res = String();
        int openBracket = 0;
        int countQuote = 0;
        for (uint i = startInd + 1; i < json.length(); i++)
        {
            auto ch = json[i];
            if (ch == '[' || ch == '{')
            {
                openBracket++;
            }
            else if (ch == ']' || ch == '}')
            {
                openBracket--;
            }
            else if (ch == '"')
            {
                countQuote++;
            }

            if ((ch == ',' && openBracket == 0) || openBracket < 0)
            {
                break;
            }

            res += ch;
        }

        res.trim();
        if (countQuote == 2)
        {
            res.replace(F("\""), F(""));
        }

        isOk = true;
        return res;
    }

    String GetJsonData(const String &json, const String &name)
    {
        bool isOk = false;
        return GetJsonData(json, name, isOk);
    }

    String BuildJson(const String *names, const String *data, int dataCount)
    {
        String json = String(F("{"));

        for (int dataInd = 0; dataInd < dataCount; dataInd++)
        {
            json += '"';
            json += names[dataInd];
            json += '"';

            json += ':';

            json += '"';
            json += data[dataInd];
            json += '"';

            if (dataInd + 1 != dataCount)
            {
                json += ',';
            }
        }

        return json + String("}");
    }
}