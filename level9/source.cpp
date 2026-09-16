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

    N *n1 = new N(5);
    N *n2 = new N(6);

    n1->setAnnotation(argv[1]);

    n2->operator+( *n1 );

    return 0;
}