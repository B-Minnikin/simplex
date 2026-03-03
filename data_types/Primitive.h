//
// Created by neb on 26/02/2026.
//

#ifndef LINEAR_PROGRAMMING_PRIMITIVE_H
#define LINEAR_PROGRAMMING_PRIMITIVE_H

static int nextFreeId = 0;

class Primitive {

public:
    Primitive() :
          id(nextFreeId++) { }

    [[nodiscard]] auto getId() const -> int {
        return id;
    }

    auto operator<(const Primitive& other) const -> bool {
        return id < other.id;
    }

protected:
    int id;
};


#endif //LINEAR_PROGRAMMING_PRIMITIVE_H
