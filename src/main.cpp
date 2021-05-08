#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

#define BLOCK_SIZE 20

class Mouse
{
  public:
    explicit Mouse() = default;

    static sf::Vector2f getPosition(const sf::Window &__relative_to)
    {
      return static_cast<sf::Vector2f>(sf::Mouse::getPosition(__relative_to));
    }

    static bool isPressedLeft()
    {
      return sf::Mouse::isButtonPressed(sf::Mouse::Left);
    }

    static bool isPressedRight()
    {
      return sf::Mouse::isButtonPressed(sf::Mouse::Right);
    }


    virtual ~Mouse() = default;
};

class Block : public sf::RectangleShape
{
  private:
    sf::RectangleShape rectangle;

    friend bool operator==(const Block& a, const Block &b)
    {
      return a.getPosition() == b.getPosition();
    }
  public:
    explicit Block(sf::Vector2f __position = sf::Vector2f(0.f, 0.f), sf::Vector2f __size = sf::Vector2f(20.f, 20.f))
    {
      this->setPosition(__position);
      this->setSize(__size);
    }

    virtual ~Block() = default;
};

class Grid : public sf::Drawable, public sf::Transformable
{
  private:
    sf::VertexArray vertex_array;
    sf::Vector2f
      window_size, grid_space;
  
    virtual void draw(sf::RenderTarget &__target, sf::RenderStates __states) const
    {
      __states.transform *= this->getTransform();
      __states.texture = nullptr;

      __target.draw(this->vertex_array, __states);
    }
  public:
    explicit Grid(const sf::Vector2f __size, const sf::Vector2f __grid_space) :
      window_size(__size),
      grid_space(__grid_space)
    {
      this->vertex_array.setPrimitiveType(sf::Lines);

      sf::Vector2f grid_size (
        this->window_size.x / this->grid_space.x,
        this->window_size.y / this->grid_space.y
      );
      
      for(int x = 0; x < grid_size.x; x++)
      {
        sf::Vertex line;
        
        line.position = sf::Vector2f(
          this->grid_space.x * x,
          0
        ); this->vertex_array.append(line);
        
        line.position = sf::Vector2f(
          this->grid_space.x * x,
          this->window_size.y
        ); this->vertex_array.append(line);
      }
      
      for(int y = 0; y < grid_size.y; y++)
      {
        sf::Vertex line;
        
        line.position = sf::Vector2f(
          0,
          this->grid_space.y * y
        ); this->vertex_array.append(line);
        
        line.position = sf::Vector2f(
          this->window_size.x,
          this->grid_space.y * y
        ); this->vertex_array.append(line);
      }
    }
    const sf::Vector2f &getGridSize()
    {
      return this->window_size;
    }

    virtual ~Grid() = default;
};

class Layout : public sf::Transformable, public sf::Drawable
{
  private:
    sf::Sprite layout_sprite;
    sf::Texture layout_texture;

    std::vector<Layout*> items;
    sf::Vector2f padding;
    sf::Vector2u items_index;

    virtual void draw(sf::RenderTarget &__target, sf::RenderStates __states) const
    {
      __states.transform *= this->getTransform();
      __states.texture = &this->layout_texture;

      __target.draw(this->layout_sprite, __states);

      if(this->items.size() > 0)
        for(auto &it : this->items)
          __target.draw(*it);
    }
  public:
    explicit Layout(sf::Vector2f __position, sf::Vector2f __size, sf::String __layout_style)
      : padding(sf::Vector2f(20.f, 20.f)), items_index(sf::Vector2f(1, 1))
    {
      this->layout_texture.loadFromFile(__layout_style);
      this->layout_sprite.setTexture(this->layout_texture);
    
      this->layout_sprite.setPosition(__position);
      this->layout_sprite.setScale(
        __size.x,
        __size.y
      );
    }

    virtual void setPadding(sf::Vector2f __padding)
    {
      this->padding = __padding;
    }

    virtual void addItem(Layout& item)
    {
      if(item.getPosition().x < this->getPosition().x)
      {
        item.setPosition(
          sf::Vector2f(
            this->getPosition().x + this->padding.x * 3 * this->items_index.x++ - 35.f,
            this->getPosition().y + this->padding.y * 3 * this->items_index.y - 35.f
          )
        );
      }
      if(item.getPosition().x + item.getSize().x > this->getPosition().x + this->getSize().x)
      {
        this->items_index.x = 1;
        item.setPosition(
          sf::Vector2f(
            this->getPosition().x + this->padding.x * 3 * this->items_index.x - 35.f,
            this->getPosition().y + this->padding.y * 3 * ++this->items_index.y - 35.f
          )
        );

      }

      this->items.push_back(&item);
    }

    const sf::Vector2f getSize() const
    {
      return sf::Vector2f (
        this->layout_sprite.getLocalBounds().width * this->layout_sprite.getScale().x,
        this->layout_sprite.getLocalBounds().height * this->layout_sprite.getScale().y
      );
    }

    bool isClicked(const sf::Window &__relative_to)
    {
      return this->layout_sprite.getGlobalBounds().contains(Mouse::getPosition(__relative_to));
    }

    // override
    virtual void setPosition(sf::Vector2f __position)
    {
      this->layout_sprite.setPosition(__position);
    }
    const sf::Vector2f &getPosition() const
    {
      return this->layout_sprite.getPosition();
    }

    virtual ~Layout() = default;
};

int main()
{
  sf::RenderWindow window(sf::VideoMode(700, 700), "TileMap");
  sf::Event event;

  sf::Clock delay_click;
  std::vector<Block> blocks;
  sf::Color block_color = sf::Color::White;
  Grid grid(
    sf::Vector2f(
      BLOCK_SIZE * 28,
      BLOCK_SIZE * 35
    ),
    sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)
  );

  Layout layout(
    sf::Vector2f(
      BLOCK_SIZE * 28,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/layout_default.png"
  );

  // items
  Layout block_green(
    sf::Vector2f(
      0,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/items/block_green.png"

  ); layout.addItem(block_green);

  Layout block_blue(
    sf::Vector2f(
      0,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/items/block_blue.png"
  ); layout.addItem(block_blue);
  
  Layout block_blue1(
    sf::Vector2f(
      0,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/items/block_blue.png"
  ); layout.addItem(block_blue1);
  Layout block_blue2(
    sf::Vector2f(
      0,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/items/block_blue.png"
  ); layout.addItem(block_blue2);

  Layout block_blue3(
    sf::Vector2f(
      0,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/items/block_blue.png"
  ); layout.addItem(block_blue3);
  Layout block_blue4(
    sf::Vector2f(
      0,
      0
    ),
    sf::Vector2f(
      1,
      1
    ),
    "res/items/block_blue.png"
  ); layout.addItem(block_blue4);








  while(window.isOpen())
  {
    while(window.pollEvent(event))
    {
      if(event.type == sf::Event::Closed)
        window.close();
    }
    if(Mouse::isPressedLeft() && delay_click.getElapsedTime() > sf::milliseconds(25))
    {
      if(block_green.isClicked(window))
        block_color = sf::Color::Green;
      else if(block_blue.isClicked(window))
        block_color = sf::Color::Blue;

      Block block(
        sf::Vector2f(
          (int) Mouse::getPosition(window).x / BLOCK_SIZE * BLOCK_SIZE,
          (int) Mouse::getPosition(window).y / BLOCK_SIZE * BLOCK_SIZE
        ),
        sf::Vector2f(
          BLOCK_SIZE,
          BLOCK_SIZE
        )
      );
      block.setFillColor(block_color);

      if(
        block.getPosition().x < grid.getGridSize().x &&
        block.getPosition().y < grid.getGridSize().y &&
        block.getPosition().x >= 0 &&
        block.getPosition().y >= 0
      ) blocks.push_back(block);
      delay_click.restart();  
    }
    else if(Mouse::isPressedRight())
    {
      Block block(
        sf::Vector2f(
          (int) Mouse::getPosition(window).x / BLOCK_SIZE * BLOCK_SIZE,
          (int) Mouse::getPosition(window).y / BLOCK_SIZE * BLOCK_SIZE
        ),
        sf::Vector2f(
          BLOCK_SIZE,
          BLOCK_SIZE
        )
      );
      if(std::count(blocks.begin(), blocks.end(), block) > 0)
        blocks.erase(std::find(blocks.begin(), blocks.end(), block));
    }

    window.clear();

    for(auto &it : blocks)
      window.draw(it);
    window.draw(grid);
    window.draw(layout);

    window.display();
  }
  return EXIT_SUCCESS;
}
