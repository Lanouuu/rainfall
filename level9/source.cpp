#include <cstdlib>
#include <cstring>

class N
{
private:
    int value;

public:
    N(int value)
    {
        this->value = value;
    }

    void setAnnotation(char *annotation)
    {
        memcpy((char *)this + 4, annotation, strlen(annotation));
    }

    int operator+(N &other)
    {
        return other.value + this->value;
    }

    int operator-(N &other)
    {
        return this->value - other.value;
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
        _exit(1);

    N *a = new N(5);
    N *b = new N(6);

    a->setAnnotation(argv[1]);

    b->operator+( *a );

    return 0;
}