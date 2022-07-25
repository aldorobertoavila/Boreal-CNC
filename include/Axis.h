enum Axis
{
    X,
    Y,
    Z
};

Axis &operator++(Axis &c)
{
    using IntType = typename std::underlying_type<Axis>::type;
    c = static_cast<Axis>(static_cast<IntType>(c) + 1);

    if (c == Axis::Z)
        c = static_cast<Axis>(0);

    return c;
}

Axis operator++(Axis &c, int)
{
    Axis result = c;
    ++c;
    
    return result;
}