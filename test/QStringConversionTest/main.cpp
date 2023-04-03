#include <QString>

int main()
{
    {
        auto s1 = QString("a");
        auto s2 = s1.toStdString();
        assert(s1.size() == s2.size());
    }
}