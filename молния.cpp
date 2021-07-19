#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    float e = 0.4;
    //насколько быстро угасает заряд

    float DownUp = 1;//   насколько сигналу легче идти вниз, чем вверх
    float RightLeft = 1;//насколько сигналу легче идти вправо, чем влево

    float PLPL = 3;//дополнительная энергия

    int w = 500;
    int h = 500;

    int wPix = 100;
    int hPix = 100;

    int render_modes = 8;//режимы рендеринга

    //расчёт вероятности случаев
    float a = DownUp;
    float b = RightLeft;
    float y = a * a * b * b / (a * a * a * b * b + a * a * b * b + a * a * a * b + a * a * a + a * b * b + b * b + a * b + a);
    if (y <= 0)
    {
        std::cout << "error, y  <= 0: y = " << y << "\n";
        exit(0);
    }
    /*
    * esc - закончить сейчас же     
    * e -   последняя попытка
    * f -   обновляй ток
    * g -   режим энергии
    * r -   режимы рендеринга
    */
    float s1 = a * y / b;
    float s2 = (a * y / b) * (1 + b);
    float s3 = (a * y / b) * (1 + b + (1 / b));
    float s4 = (a * y / b) * (1 + b + (1 / b)) + (y / a / a);
    float s5 = (a * y / b) * (1 + b + (1 / b)) + (y / a / a) + y;
    float s6 = (a * y / b) * (1 + b + (1 / b) + (b / a / a)) + (y / a / a) + y;
    float s7 = (a * y / b) * (1 + b + (1 / b) + (b / a / a) + (1 / a / a)) + (y / a / a) + y;

    std::cout << "s1 = " << s1 << "\n";
    std::cout << "s2 = "  << s2 << "\n";
    std::cout << "s3 = " << s3 << "\n";
    std::cout << "s4 = " << s4 << "\n";
    std::cout << "s5 = " << s5 << "\n";
    std::cout << "s6 = " << s6 << "\n";
    std::cout << "s7 = " << s7 << "\n";

    sf::RenderWindow window(sf::VideoMode(w, h), "molnya", sf::Style::None);

    window.clear();
    //клетка установка размера навсегда
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(w / wPix, h / hPix));
    rectangle.setFillColor(sf::Color::White);

    window.setMouseCursorVisible(false);

    sf::Texture texture1;
    texture1.create(w, h);

    sf::Texture texture;
    texture.create(w, h);
    sf::Sprite sprite(texture);

    float seed;

    srand(time(NULL));
    a = rand() % 1000;
    b = rand() % 1000;
    int flip_len;
    srand(a * 1000 + b);

    int kadr = -1;

    float** F = new float* [wPix];
    bool end_b = false;
    bool flp = false;
    bool energy = false;
    int rendering = 1;
    float psum;
    //F[ширина][высота]
    for (int i = 0; i < wPix; i++)
    {
        F[i] = new float[hPix];
        for (int j = 0; j < hPix; j++)
        {
            F[i][j] = 0;
        }
    }

    bool** Graf = new bool* [wPix];
    //Graf[ширина][высота]
    for (int i = 0; i < wPix; i++)
    {
        Graf[i] = new bool[hPix];
        for (int j = 0; j < hPix; j++)
        {
            Graf[i][j] = false;
        }
    }

    F[int(wPix / 2)][0] = 1;

    int** flips = new int* [wPix * hPix];//интересные точки (заряженные)
    for (int i = 0; i < (wPix * hPix); i++)
    {
        flips[i] = new int[2];
        flips[i][0] = -1;
        flips[i][1] = -1;
    }
    //это для оптимизации

    flips[0][0] = int(wPix / 2);
    flips[0][1] = 0;

    rectangle.setPosition(int(wPix / 2) * w / wPix, 0);
    window.draw(rectangle);
    window.display();

    int g;
    float sum;

    float random_ver;

    window.clear();

    Graf[flips[0][0]][flips[0][1]] = true;

    bool logik;
    int best_h = 0;

    std::cout << "Go" << "\n";
    int c;
    bool end_while = false;

    while (window.isOpen() && (!end_while))
    {
        kadr++;

        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::KeyPressed)
            {
                if (evnt.key.code == sf::Keyboard::Escape)
                {
                    end_while = true;
                }
                else if (evnt.key.code == sf::Keyboard::E)
                {
                    end_b = true;
                    std::cout << "end-mode" << "\n";
                }
                else if (evnt.key.code == sf::Keyboard::F)
                {
                    flp = true;
                    std::cout << "flip-mode" << "\n";
                }
                else if (evnt.key.code == sf::Keyboard::G && energy)
                {
                    energy = false;
                    std::cout << "standart-mode" << "\n";
                }
                else if (evnt.key.code == sf::Keyboard::G && (!energy))
                {
                    energy = true;
                    std::cout << "energy-mode" << "\n";
                }
                else if (evnt.key.code == sf::Keyboard::R)
                {
                    rendering += 1;
                    std::cout << "render: " << rendering << " kadrs" << "\n";
                }
            }
        }
        if (rendering >= render_modes)
        {
            rendering = 1;
        }
        //пробивы
        sum = 0;
        flip_len = 0;
        for (int i = 0; i < wPix * hPix; i++)
        {
            if (flips[i][0] == -1)
            {
                break;
            }
            else
            {
                sum += F[flips[i][0]][flips[i][1]];
            }
            flip_len += 1;
        }

        random_ver = float(rand()) / 100000;
        psum = 0;
        for (int i = 0; i < wPix * hPix; i++)
        {
            if (flips[i][0] == -1)
            {
                break;
            }
            else if (sum == 0)
            {
                break;
            }
            else if (random_ver <= (F[flips[i][0]][flips[i][1]] / sum + psum))
            {
                //пробив
                random_ver = float(rand()) / 100000;
                if (random_ver <= s1 && (flips[i][1] + 1 <= hPix))
                {
                    F[flips[i][0]][flips[i][1] - 1] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0];//тут всё хорошо вроде
                    flips[flip_len][1] = flips[i][1] + 1;
                }
                else if (random_ver <= s2 && (flips[i][1] + 1 <= hPix) && (flips[i][0] - 1 >= 0))
                {
                    F[flips[i][0] - 1][flips[i][1] - 1] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0] - 1;
                    flips[flip_len][1] = flips[i][1] + 1;
                }
                else if (random_ver <= s3 && (flips[i][1] + 1 >= 0) && (flips[i][0] + 1 <= wPix))
                {
                    F[flips[i][0] + 1][flips[i][1] - 1] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0] + 1;
                    flips[flip_len][1] = flips[i][1] + 1;
                }
                else if (random_ver <= s4 && (flips[i][0] - 1 >= 0))
                {
                    F[flips[i][0] - 1][flips[i][1]] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0] - 1;
                    flips[flip_len][1] = flips[i][1];
                }
                else if (random_ver <= s5 && (flips[i][1] + 1 <= hPix))
                {
                    F[flips[i][0] + 1][flips[i][1]] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0] + 1;
                    flips[flip_len][1] = flips[i][1];
                }
                else if (random_ver <= s6 && (flips[i][0] - 1 >= 0) && (flips[i][1] - 1 >= 0))
                {
                    F[flips[i][0] - 1][flips[i][1] + 1] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0] - 1;
                    flips[flip_len][1] = flips[i][1] - 1;
                }
                else if (random_ver <= s7 && (flips[i][1] - 1 >= 0))
                {
                    F[flips[i][0]][flips[i][1] + 1] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0];
                    flips[flip_len][1] = flips[i][1] - 1;
                }
                else if ((flips[i][0] + 1 <= wPix) && (flips[i][1] - 1 >= 0))
                {
                    F[flips[i][0] + 1][flips[i][1] + 1] += F[flips[i][0]][flips[i][1]];
                    flips[flip_len][0] = flips[i][0] + 1;
                    flips[flip_len][1] = flips[i][1] - 1;
                }
                F[flips[flip_len][0]][flips[flip_len][1]] += PLPL;
                Graf[flips[flip_len][0]][flips[flip_len][1]] = true;

                break;
            }
            else
            {
                sum -= F[flips[i][0]][flips[i][1]];
            }
            psum += F[flips[i][0]][flips[i][1]] / sum;
        }

        //удаление электрического заряда
        for (int i = 0; i < wPix * hPix; i++)
        {
            if (flips[i][0] == -1)
            {
                break;
            }
            else if (F[flips[i][0]][flips[i][1]] >= e)
            {
                F[flips[i][0]][flips[i][1]] = F[flips[i][0]][flips[i][1]] - (e);
            }
            else
            {
                F[flips[i][0]][flips[i][1]] = 0;
                g = -1;
                while (true)
                {
                    g += 1;
                    //так и должно быть, что оно впереди
                    flips[i + g][0] = flips[i + g + 1][0];
                    flips[i + g][1] = flips[i + g + 1][1];

                    if (flips[i + g][0] == -1)
                    {
                        break;
                    }
                }
            }
        }

        if (true)
        {
            for (int i = 0; i < wPix; i++)
            {
                for (int j = 0; j < hPix; j++)
                {
                    if (energy)
                    {
                        if (F[i][j] * 200 < 160)
                        {
                            rectangle.setFillColor(sf::Color::Color(120, 120, 255));
                        }
                        else
                        {
                            rectangle.setFillColor(sf::Color::Color(F[i][j] * 255, 0, 0));
                        }
                    }
                    else
                    {
                        rectangle.setFillColor(sf::Color::Color(120, 120, 255));
                    }

                    if (Graf[i][j])
                    {
                        rectangle.setPosition(i * w / wPix, j * h / hPix);
                        window.draw(rectangle);
                    }
                }
            }
            window.display();
        }

        if (flip_len == 0)
        {
            std::cout << "end" << "\n";
            logik = false;
            for (int i = 0; i < wPix; i++)
            {
                if (Graf[i][hPix - 1])
                {
                    logik = true;
                }
            }
            for (int i = 0; i < hPix; i++)
            {
                if (Graf[wPix - 1][i] || Graf[0][i])
                {
                    logik = true;
                }
            }
            if (logik || end_b)
            {
                break;
            }
            else if (flp)
            {
                best_h = -1;
                c = 0;
                for (int i = 0; i < wPix; i++)
                {
                    for (int j = 0; j < hPix; j++)
                    {
                        if ((best_h < j) && (Graf[i][j])) 
                        {
                            best_h = j;
                        }
                        //F[bestx][besty] = 0;
                    }
                }
                for (int i = 0; i < wPix; i++)
                {
                    if (Graf[i][best_h])
                    {
                        F[i][best_h] += 1 + PLPL;
                        flips[c][0] = i;
                        flips[c][1] = best_h;
                        c++;
                    }
                }
            }
            else
            {
                std::cout << "restart" << "\n";
                window.close();
                main();
                return 0;
            }
        }
    }
    
    texture1.update(window);

    texture1.copyToImage().saveToFile("молния.png");

    while (window.isOpen())
    {
        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::KeyPressed)
            {
                //это возможность посмотреть на изображение после выхода
                if (evnt.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }
    }

    window.setMouseCursorVisible(true);

    return 0;
}