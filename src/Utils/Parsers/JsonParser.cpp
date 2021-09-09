#include "JsonParser.h"

namespace JsonParser
{
    String GetJsonData(String &json, String name, bool &isOk)
    {
        name = String("\"") + name + String("\"");

        auto dataStart = json.indexOf(name);
        if (dataStart == -1)
        {
            isOk = false;
            return "";
        }

        int startInd = json.indexOf(':', dataStart);
        if (startInd == -1)
        {
            isOk = false;
            return "";
        }

        String res = "";
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
            res.replace("\"", "");
        }

        isOk = true;
        return res;
    }

    String GetJsonData(String &json, String name)
    {
        bool isOk = false;
        return GetJsonData(json, name, isOk);
    }

    String BuildJson(String *names, String *data, int dataCount)
    {
        String json = "{";

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

        return json + "}";
    }
}