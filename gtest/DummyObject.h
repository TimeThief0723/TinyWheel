#pragma once

class OjbectOnlyMove{
public:
    OjbectOnlyMove(){}
    OjbectOnlyMove(const OjbectOnlyMove &o) = delete;
    OjbectOnlyMove(OjbectOnlyMove &&o) {}
    OjbectOnlyMove &operator=(const OjbectOnlyMove &) = delete;
    OjbectOnlyMove &operator=(OjbectOnlyMove &&) { return *this; }
};

class OjbectOnlyCopy{
public:
    OjbectOnlyCopy(){}
    OjbectOnlyCopy(const OjbectOnlyCopy &o) {}
    OjbectOnlyCopy(OjbectOnlyCopy &&o) = delete;
    OjbectOnlyCopy &operator=(const OjbectOnlyCopy &) { return *this; }
    OjbectOnlyCopy &operator=(OjbectOnlyCopy &&) = delete;
};

