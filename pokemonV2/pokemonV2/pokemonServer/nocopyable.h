#ifndef NOCOPYABLE_H
#define NOCOPYABLE_H

namespace fivestar
{
    class nocopyable
    {
    private:
        nocopyable(const nocopyable& x) = delete;
        nocopyable& operator=(const nocopyable&x) = delete;
    public:
        nocopyable() = default;
        ~nocopyable() = default;
    };

}

#endif // NOCOPYABLE_H