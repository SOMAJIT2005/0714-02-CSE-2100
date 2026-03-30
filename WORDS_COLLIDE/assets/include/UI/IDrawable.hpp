#ifndef I_DRAWABLE_HPP
#define I_DRAWABLE_HPP

class Renderer; // Forward declaration

class IDrawable {
public:
    virtual ~IDrawable() = default;
    virtual void render(Renderer& renderer) const = 0;
};

#endif // I_DRAWABLE_HPP