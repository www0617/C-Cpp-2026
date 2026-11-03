#include <iostream>
#include <memory>
#include <vector>

// Zoo: base class Animal + polymorphism, make all animals speak in order.
// (1) hold any number of animals (at least Dog, Cat), each with its own sound
// (2) add Bird and WolfDog (a dog that howls) WITHOUT changing the Zoo class

// abstract base class: pure virtual speak, each subclass implements its own sound
class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() const = 0;
};

class Dog : public Animal {
public:
    void speak() const override { std::cout << "Dog: woof\n"; }
};

class Cat : public Animal {
public:
    void speak() const override { std::cout << "Cat: meow\n"; }
};

// (2) new: bird
class Bird : public Animal {
public:
    void speak() const override { std::cout << "Bird: tweet\n"; }
};

// (2) new: wolf dog, inherits Animal directly (not Dog), sounds like a howl
class WolfDog : public Animal {
public:
    void speak() const override { std::cout << "WolfDog: howl\n"; }
};

// Zoo only knows the Animal base pointer, so adding new animals needs no change here
class Zoo {
public:
    void add(std::unique_ptr<Animal> animal) {
        animals_.push_back(std::move(animal));
    }

    void makeAllSounds() const {
        for (const auto& a : animals_) a->speak();
    }

private:
    std::vector<std::unique_ptr<Animal>> animals_;
};

int main() {
    Zoo zoo;
    zoo.add(std::make_unique<Dog>());
    zoo.add(std::make_unique<Cat>());
    zoo.add(std::make_unique<Bird>());
    zoo.add(std::make_unique<WolfDog>());

    zoo.makeAllSounds();
    return 0;
}
