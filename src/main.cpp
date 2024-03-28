/* 使用SFML来实现一下贪吃蛇 */

#include <SFML/Graphics.hpp>
#include <SFML/Audio/Export.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <utility>  // for use of pair
#include <string>
#include <iostream>
#include <cstdlib>  
#include <random>
#include <cmath>

#define WIN_WIDTH  640  // 游戏窗口大小
#define WIN_HEIGHT 640
#define GRID_LENGTH 20  // 每个格点的边长
#define HORIZONTAL_BOUND (WIN_WIDTH - GRID_LENGTH) // snake所能到达的边界 HORIZONTAL_BOUND 是游戏窗口宽度减去一个格点的边长
#define VERTICAL_BOUND  (WIN_HEIGHT - GRID_LENGTH)  
#define SLOW_SPEED (1.0 / 5) //SLOW_SPEED 是每秒移动5次
#define FAST_SPEED (1.0 / 10) //FAST_SPEED 是每秒移动10次。

using namespace std;//命名空间

bool float_equal(float a, float b) //计算浮点的误差
{
    const float EPSILON = 1e-5;
    if (fabs(a - b) < EPSILON)
        return true;
    return false;
}

/* 实现细节:
    snake_中存储snake的头,身体,每一个部分是一个元素.
    每个位置都表示一个格点左上角的坐标.
    如果snake咬到自己或者撞了墙,设置died_为true,游戏结束.
 */
//SnakeGame 类，负责整个游戏的管理。包括游戏初始化、事件处理、更新游戏状态和渲染。
class SnakeGame {
public:
    SnakeGame(); //构造方法
    void Run();

private:
    //游戏初始化、事件处理、更新游戏状态和渲染。
    void ProcessEvents(); 
    void Update();
    void Render();

    // 负责画一个格点(头或者身体)
    void DrawOneCell(const pair<float, float> &pos, bool is_head,int x);

    //接收键盘消息，进行处理
    void HandleInput(sf::Keyboard::Key key);

    // 将果实(star)放到一个新的位置,并且不和snake身体重合 使用随机数生成新的果实位置，确保不与贪吃蛇的身体重合。
    void ChangeStar();

    // 给定(x, y),如果是snake的一部分,返回true
    bool InBody(float x, float y);
    
    // 检查是否game over 包括贪吃蛇是否撞墙或咬到自己
    void CheckDied();

private:
    enum Direction { LEFT, UP, RIGHT, DOWN };  //这个enum用来存储移动方向
        
    //这些变量用于存储游戏状态和游戏元素，以及用于渲染游戏界面的图形对象和窗口对象。
private:
    vector<pair<float, float>> snake_; //存储蛇的相关点
    Direction next_direction_;  //用来存储蛇下一步移动的方向
    Direction previous_direction_;  //用来存储蛇上一步移动的方向
    bool died_;  //表示游戏是否结束。如果蛇撞到边界或者撞到自己的身体，则游戏结束，died_ 变量会被设置为 true。
    sf::Time SECONDS_PER_FRAME; //表示每一帧的持续时间。它用于控制游戏的帧率，从而控制游戏的运行速度。

    sf::Sprite head_;  //用来表示蛇头
    sf::Sprite obody_;  // 奇数蛇身体
    sf::Sprite ebody_; //偶数蛇身
    sf::Sprite star_;  //   星星点（即食物）  
    sf::Texture head_texture_;  // 存储蛇头
    sf::Texture obody_texture_;  // 蛇身体
    sf::Texture ebody_texture_;  // 蛇身体
    sf::Texture star_texture_;  //星星的纹理
    sf::RenderWindow window_;   //一个 SFML 窗口对象，用于创建和管理游戏的图形界面
    sf::Font font; //字体

    
}; 

SnakeGame::SnakeGame()
    //初始化了 next_direction_ 成员变量，将其设置为 RIGHT。next_direction_ 表示蛇下一步移动的方向，在这里初始化为 RIGHT，表示初始时蛇的方向为向右移动。
    : next_direction_(RIGHT),
    //初始化了 previous_direction_ 成员变量，将其设置为 RIGHT。previous_direction_ 表示蛇上一步移动的方向，在这里初始化为 RIGHT，表示初始时蛇的方向为向右移动。
      previous_direction_(RIGHT),
      //表示游戏没有结束
      died_(false),
      //当前游戏的速度
      SECONDS_PER_FRAME(sf::seconds(SLOW_SPEED)),
      //窗口的大小和名字
      window_(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), L"贪吃蛇") 
      
      
{

    snake_.push_back({100, 300});
    snake_.push_back({80, 300});
    snake_.push_back({60, 300});

    //加载字体
    if (!font.loadFromFile("assets/fonts/SourceHanSansSC-Bold.otf")) {
        std::cerr << "Failed to load font file" << std::endl;
    }
    //图片资源加载
    string head_file_path = "pic/head.png";
    string obody_file_path = "pic/obody.png";
    string ebody_file_path = "pic/ebody.png";
    string star_file_path = "pic/star.png";

    //加载图标
    sf::Image icon;
    icon.loadFromFile("pic/favicon.png"); // 加载图标
    window_.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    //这里提示图片是否加载成功 并且加到Texture中
    if (!head_texture_.loadFromFile(head_file_path))
        cout << "Load head failed.\n";
    head_.setTexture(head_texture_);
    if (!obody_texture_.loadFromFile(obody_file_path))
        cout << "Load obody failed.\n";
    obody_.setTexture(obody_texture_);
    if (!ebody_texture_.loadFromFile(ebody_file_path))
        cout << "Load ebody failed.\n";
    ebody_.setTexture(ebody_texture_);
    if (!star_texture_.loadFromFile(star_file_path))
        cout << "Load star failed.\n";
    if (!icon.loadFromFile("path_to_icon.png")) {
        cout<< "Load icon failed.\n";
    }
    star_.setTexture(star_texture_);
    

    ChangeStar();  // 初始食物的随机化位置
}


//绘画头和食物
//pos：一个包含浮点型坐标对的 pair，表示要绘制的格点的位置。 is_head：一个布尔值，指示要绘制的格点是否为蛇的头部。
void SnakeGame::DrawOneCell(const pair<float, float> &pos, bool is_head,int x)
{
    sf::Sprite *sprite;
    if (is_head)
        {
            sprite = &head_; //如果是头部就绘画头
        }
    else //分为奇数和偶进行加载不同的身体，形成间隔的身体
    {
        if ( x % 2 == 0)
        {
            sprite = &ebody_;
        }
        else
        {
            sprite = &obody_;
        }
    }
    sprite->setPosition(pos.first + 1, pos.second + 1); //添加设的格子
    window_.draw(*sprite); //展示出来
}
//用于渲染游戏画面
void SnakeGame::Render()
{
    if (died_) {
        // 如果已经die了
        if (snake_.size() < 10)
            cout << "You lost.\n";
        else if (snake_.size() < 20)
            cout << "Great!\n";
        else
            cout << "Amazing!\n";
        cout << "Final length: " << snake_.size() << endl;
        window_.close();
        return;
    }
    window_.clear();


    //设置背景颜色和边框颜色
    sf::RectangleShape rectangle(sf::Vector2f(10.f, 10.f));
    double frameLength = 20.0;
    
    auto size = window_.getSize();
    rectangle.setPosition(frameLength, frameLength); //设置起点
    rectangle.setSize(sf::Vector2f(size.x - frameLength * 2, size.y - frameLength * 2));//这个是长方形的大小
    rectangle.setFillColor(sf::Color(230, 230, 230));
    rectangle.setOutlineThickness(frameLength); //边框的大小
    rectangle.setOutlineColor(sf::Color(125, 0, 25));
    window_.draw(rectangle);
    
    sf::Text text;
    text.setFont(font);
    text.setString(sf::String(L"目前得分:") + std::to_string(snake_.size()-3));
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setPosition(250.f, 20.f);

    //这个加一个参数，使得
    int oedigit = 0 ;
    auto it = snake_.begin();
    if (it != snake_.end())
        DrawOneCell(*it, true,++oedigit);
    while (++it != snake_.end())
        DrawOneCell(*it, false,++oedigit);
    window_.draw(text);
    window_.draw(star_);
    window_.display();
}

//更新
void SnakeGame::Update()
{
    auto &first = snake_[0];  // 获取蛇的头部
    pair<float, float> new_head = first; //创建一个新的头部坐标 new_head，并将其初始化为当前头部的坐标。
    
    //这段代码用来检查蛇的移动方向是否与上一个移动方向相反。如果是相反方向，则将蛇的下一个移动方向设置为与上一个相同的方向。这样可以避免蛇立即掉头移动。
    if (next_direction_ - previous_direction_ == -2 ||
        next_direction_ - previous_direction_ == 2) {
        next_direction_ = previous_direction_;
    }
    /*如果 next_direction_ 为 LEFT，则将新头部的横坐标向左移动一个格子的长度 GRID_LENGTH。
    如果 next_direction_ 为 RIGHT，则将新头部的横坐标向右移动一个格子的长度 GRID_LENGTH。
    如果 next_direction_ 为 UP，则将新头部的纵坐标向上移动一个格子的长度 GRID_LENGTH。
    否则，将新头部的纵坐标向下移动一个格子的长度 GRID_LENGTH。*/
    if (next_direction_ == LEFT)
        new_head.first -= GRID_LENGTH;
    else if (next_direction_ == RIGHT)
        new_head.first += GRID_LENGTH;
    else if (next_direction_ == UP)
        new_head.second -= GRID_LENGTH;
    else
        new_head.second += GRID_LENGTH;
    snake_.insert(snake_.begin(), new_head); //把新加的点，插入到蛇的尾部


    // 检查有没有挂
    CheckDied();
    if (died_) {
        return;
    }
    auto star_pos = star_.getPosition();//获取星星的位置，存储在变量 star_pos 中。这个位置是星星对象 star_ 的坐标。

    //检查蛇的头部是否与星星重合。如果头部与星星重合，表示蛇吃到了星星，调用函数 ChangeStar() 来改变星星的位置
    if (float_equal(new_head.first, star_pos.x) &&
        float_equal(new_head.second, star_pos.y)) {
        ChangeStar();
    } else {
        snake_.pop_back();//如果蛇的头部没有与星星重合，表示蛇没有吃到星星。这种情况下，蛇的尾部会被移除，即删除蛇的最后一个元素，以模拟蛇的移动。
    }
    previous_direction_ = next_direction_;//将上一步的移动方向更新为当前的移动方向。这样可以确保在下一次移动时参考到正确的移动方向。
}

//运行游戏
void SnakeGame::Run()
{
    int i = 0;
    sf::Clock clock; 
    sf::Time timeSinceLastUpdate = sf::Time::Zero; // 用于刷新屏幕时间 初始化了一个用于跟踪上一次更新之后经过的时间的变量 timeSinceLastUpdate，初始值为零。
    while (window_.isOpen()) {
        ProcessEvents();
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= SECONDS_PER_FRAME)
        {
            ProcessEvents();
            Update();
            timeSinceLastUpdate -= SECONDS_PER_FRAME; //将已经处理的时间减去一帧的时间，以便进行下一次的更新。
        }
        Render();
    }
}
//用于处理游戏中的事件，主要是与窗口相关的事件
void SnakeGame::ProcessEvents()
{
    sf::Event e; //窗口的事件
    while (window_.pollEvent(e)) {
        if (e.type == sf::Event::Closed||e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) { //加了在按下esc关闭游戏
            window_.close();
            return;
        }
        if (e.type == sf::Event::KeyPressed) //检查是否按下了键盘上的按键。如果是按键按下事件，则调用 HandleInput() 函数处理按键事件。
        {
            HandleInput(e.key.code);
        }
        if (e.type == sf::Event::KeyReleased)  //检查是否释放了键盘上的按键。如果是按键释放事件，则检查释放的键是否是空格键。
            if (e.key.code == sf::Keyboard::Space ||e.key.code == sf::Keyboard::Up ||e.key.code == sf::Keyboard::Left ||
            e.key.code == sf::Keyboard::Down  ||e.key.code == sf::Keyboard::Right) //如果释放的键是空格键，则将游戏的帧率恢复为慢速，即 SECONDS_PER_FRAME 设置为 SLOW_SPEED。
                SECONDS_PER_FRAME = sf::seconds(SLOW_SPEED);
    }
}

//对于键盘消息进行处理
void SnakeGame::HandleInput(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::W || key == sf::Keyboard::Up)
        next_direction_ = UP;
    else if (key == sf::Keyboard::A || key == sf::Keyboard::Left)
        next_direction_ = LEFT;
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down)
        next_direction_ = DOWN;
    else if (key == sf::Keyboard::D || key == sf::Keyboard::Right)
        next_direction_ = RIGHT;
    if (key == sf::Keyboard::Space|| key == sf::Keyboard::Right|| key == sf::Keyboard::Down||key == sf::Keyboard::Left|| key == sf::Keyboard::Up) //长按进行加速
        SECONDS_PER_FRAME = sf::seconds(FAST_SPEED);
}

// 使用最为简单的算法,每次生成一个32*32的随机数,
// 如果在身体内,则重新生成,直到合理.
//ChangeStar() 方法中，确保生成的新果实不会与贪吃蛇的身体重叠，以保证游戏的规则正确性。
void SnakeGame::ChangeStar() //3.22改进，每次重启后产生的点不同。
{
    float x, y;
    static std::random_device rd;  // 使用随机设备获取种子
    static std::default_random_engine e(rd());  // 使用种子初始化随机数引擎
    static std::uniform_int_distribution<unsigned> u(1, 30); // 31是每行格点数
    while (true) {
        // 生成随机数并检查
        x = (float)u(e) * 20;
        y = (float)u(e) * 20;
        if (!InBody(x, y))
            break;
    }
    star_.setPosition(x, y);
}

// 使用float_equal(float,float)用来判断坐标 (x, y) 是否位于蛇的身体内部。
bool SnakeGame::InBody(float x, float y)
{
    for (auto it = snake_.begin(); it != snake_.end(); ++it)
        if (float_equal(x, it->first) && float_equal(y, it->second))
            return true;
    return false;
}

//CheckDied()函数用于检查游戏是否结束，包括贪吃蛇是否撞墙或咬到自己
void SnakeGame::CheckDied()
{
    auto &new_head = snake_[0];
    // snake 撞墙了
    if (new_head.first < 1 || new_head.first > HORIZONTAL_BOUND-1 ||
        new_head.second < 1 || new_head.second > VERTICAL_BOUND-1) {
        died_ = true;
        return;
    }
    // 遍历一次看看是否有点和自己存储点重合 咬到自己
    auto it_head = snake_.begin();
    auto it_body = snake_.begin();
    while (++it_body != snake_.end()) {
        if (float_equal(it_body->first, it_head->first) &&
            float_equal(it_body->second, it_head->second)) {
            died_ = true;
            snake_.pop_back();  // 为了让snake的长度正确
            return;
        }
    }
}

int main()
{
    SnakeGame game;
    game.Run();

    return 0;
}
