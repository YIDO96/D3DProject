#pragma once
class Grid : public Actor
{
public:
    static Grid* Create(string name = "Grid");
private:
    Grid();
    virtual ~Grid();
public:
    void	Update() override;
};

