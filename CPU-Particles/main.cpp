#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <fstream>

using namespace sf;
using namespace std;

constexpr auto SCREEN_WIDTH  = 1280;
constexpr auto SCREEN_HEIGHT = 640;

const float particle_radius_2x = 4;

// they all have the same mass
const float G_mul_m = 1;


struct Particle
{
    Vector2f position;
    Vector2f velocity;
};

int main()
{
    // prepare window
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "");
    window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(30);

    const int len = 400;
    const float blue_part = 0.0;
    const float white_part = 1.0;

    // random fill particles array
    Particle* particles = new Particle[len];
    for (int i = 0; i < len; i++)
    {
        particles[i].position.x = rand() % (SCREEN_WIDTH - 1);
        particles[i].position.y = rand() % (SCREEN_HEIGHT - 1);

        particles[i].velocity.x = ((rand() % 20) - 10.0) / 20.0;
        particles[i].velocity.y = ((rand() % 20) - 10.0) / 20.0;
    }

    glPointSize(2);

    // specify open gl coordinate system
    glOrtho(-SCREEN_WIDTH * 0.5, SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5, -SCREEN_HEIGHT * 0.5, 0.0f, 1.0f);
    glScalef(1, -1, 1);
    glTranslatef(-(SCREEN_WIDTH * 0.5f), -(SCREEN_HEIGHT * 0.5f), 0.0f);

    // main loop
    while (window.isOpen())
    {
        // check close event (also here a delay is being made to fix fps)
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        // for each pair of points
        for (int i = 0; i < len-1; i++)
        {
            Vector2f& p1 = particles[i].position;
            for (int j = i+1; j < len; j++)
            {
                Vector2f& p2 = particles[j].position;

                // calculate accelerations and update velocitys
                Vector2f delta = p2 - p1; // direction from p1 to p2
                float dist = sqrt(pow(int(delta.x), 2) + pow(int(delta.y), 2));

                if (dist > particle_radius_2x)
                {
                    Vector2f a2d = (delta * G_mul_m) / pow(dist, 3);

                    if (i < len * blue_part)  // first blue
                    {
                        if (j < len * blue_part) // second blue
                        {
                            particles[i].velocity -= a2d;
                            particles[j].velocity += a2d;
                        }
                        else if ((i >= len * blue_part) && (i < len * (blue_part + white_part))) // second white
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                        else // second red
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                    }
                    else if ((i >= len * blue_part) && (i < len * (blue_part + white_part))) // first white
                    {
                        if (j < len * blue_part) // second blue
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                        else if ((i >= len * blue_part) && (i < len * (blue_part + white_part))) // second white
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                        else // second red
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                    }
                    else // first red
                    {
                        if (j < len * blue_part) // second blue
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                        else if ((i >= len * blue_part) && (i < len * (blue_part + white_part))) // second white
                        {
                            particles[i].velocity += a2d;
                            particles[j].velocity -= a2d;
                        }
                        else // second red
                        {
                            particles[i].velocity -= a2d;
                            particles[j].velocity += a2d;
                        }
                    }
                }
                else if (dist > 0)
                {
                    Vector2f a2d = (delta * G_mul_m) / pow(dist, 2);

                    particles[i].velocity -= a2d;
                    particles[j].velocity += a2d;
                }
            }
        }

        // redraw particles by direct open gl calls
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_POINTS);

        double U = 0;

        {
            for (int i = 0; i < len; i++)
            {
                // observe velocity limit
                float speed_module = sqrt(pow(particles[i].velocity.x, 2) + pow(particles[i].velocity.y, 2));
                if (speed_module > 1.0) 
                {
                    particles[i].velocity /= speed_module;
                    speed_module = 1.0;
                }

                // add to U
                U += speed_module;

                // check is point cross any border
                Vector2i next_pos = static_cast<Vector2i>(particles[i].position + particles[i].velocity);

                if (next_pos.x > SCREEN_WIDTH - 2)
                {
                    particles[i].velocity.x -= 2 * particles[i].velocity.x;
                }
                else if (next_pos.x < 1)
                {
                    particles[i].velocity.x -= 2 * particles[i].velocity.x;
                }

                if (next_pos.y > SCREEN_HEIGHT - 2)
                {
                    particles[i].velocity.y -= 2 * particles[i].velocity.y;
                }
                else if (next_pos.y < 1)
                {
                    particles[i].velocity.y -= 2 * particles[i].velocity.y;
                }

                // move
                particles[i].position += particles[i].velocity;

                // display particle
                {
                    // draw blue part
                    if (i < len * blue_part)
                    {
                        glColor3f(0, 0, 1);
                        glVertex2i(int(particles[i].position.x), int(particles[i].position.y));
                    }

                    // draw white part
                    else if (i < len * (blue_part + white_part))
                    {
                        // red on velocity high
                        glColor3f((float)speed_module, 1 - (float)speed_module, 1 - (float)speed_module); // speed_module should be <= 1
                        glVertex2i(int(particles[i].position.x), int(particles[i].position.y));
                    }

                    // draw red part
                    else
                    {
                        glColor3f(1, 0, 0);
                        glVertex2i(int(particles[i].position.x), int(particles[i].position.y));
                    }
                }
            }
        }
        glEnd();
        window.display();

        // print U
        window.setTitle("U = " + to_string(U));
    }

    return 0;
}