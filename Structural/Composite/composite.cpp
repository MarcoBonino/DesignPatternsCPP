#include <iostream>
#include <string>
#include <vector>

class File
{
public:
    explicit File(const std::string& name_)
        : name(name_)
    {}

    virtual ~File() = default;

    std::string getName() { return name; }
    void setName(const std::string& new_name) { name = new_name; }

    virtual void tree(unsigned level = 0)
    {
        printIndentation(level);
        std::cout << name << std::endl;
    }

protected:
    void printIndentation(unsigned level)
    {
        if (level > 0)
        {
            for (auto lev = 0; lev < level-1; ++lev)
                std::cout << "|" << std::string(level * 2, ' ');

            std::cout << "|-- ";
        }
    }
    std::string name;
};

/* This is the Composite */
class Directory : public File
{
public:
    explicit Directory(const std::string& name_)
        : File(name_)
    {}

    void tree(unsigned level = 0) override
    {
        printIndentation(level);
        std::cout << name << std::endl;
        for (auto&& f : files)
            f->tree(level+1);
    }

    void addFile(File* f)
    {
        files.push_back(f);
    }

private:
    std::vector<File*> files;
 };


int main()
{
    File f1("file_1");
    File f2("file_2");
    File f3("file_3");
    File f4("file_4");

    Directory d1("dir1");
    Directory d2("dir2");
    Directory d3("dir3");

    d1.addFile(&d2);
    d1.addFile(&d3);
    d1.addFile(&f4);

    d2.addFile(&f1);
    d2.addFile(&f2);
    d2.addFile(&f3);

    d1.tree();

    return 0;
}