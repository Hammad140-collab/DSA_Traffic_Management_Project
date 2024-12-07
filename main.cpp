#include<iostream>
#include<fstream>
#include<sstream>
#include<cmath>
#include<SFML/Graphics.hpp>
using namespace std;

//Constant Macros And Global Variables
const float REPULSION_FORCE = 10000.0f;
const float DAMPING = 0.85f;
const float ATTRACTION_FORCE = 0.05f;
const string fontPath = R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\Arial.ttf)";
int number_of_intersections = 0;
int number_of_roads = 0;


//Road Network and Graph
class RoadNetwork {
    public:
    RoadNetwork() {
        intersections = nullptr;
        roads = nullptr;
    }
    struct Node {
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::CircleShape shape;
        sf::Text text;
        char symbol;

        Node(){}

        Node(float x, float y, char symbol, const sf::Font& font)
            : position(x, y), velocity(0.0f, 0.0f), symbol(symbol) {
            // Initialize the circle shape
            shape.setRadius(20.0f);
            shape.setFillColor(sf::Color::Cyan);
            shape.setOrigin(20.0f, 20.0f); // Center the circle
            shape.setPosition(position);

            // Initialize the text
            text.setFont(font);
            text.setString(symbol);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);

            // Center the text inside the circle
            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                           textBounds.top + textBounds.height / 2.0f);
            text.setPosition(position);
        }

        // Update node position and synchronize visuals
        void update() {
            position += velocity;
            shape.setPosition(position);
            text.setPosition(position);
            velocity *= DAMPING; // Apply damping to slow down movement
        }
    }*intersections;
    struct Edge {
        int from;
        int to;
        int weight;

        Edge() {
            from = 0;
            to = 0;
            weight = 0;
        }
        Edge(const unsigned char from, const unsigned char to, const int weight) {
            this->from = from - 65;
            this->to = to - 65;
            this->weight = weight;
        }
        void print()const {
            cout << "from: " << from << " to: " << to << " weight: " << weight << endl;
        }
        unsigned char getfrom()const {
            return static_cast<unsigned char>(from+65);
        }

    }*roads;
    void loadRoadData() {
        fstream file(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\road_network.csv)",ios::in);
        if(!file.is_open()) {
            cout<<"File could not be opened"<<endl;
            return;
        }
        string line;
        while(getline(file,line)) {
            number_of_roads++;
        }
        number_of_roads--;
        roads = new Edge[number_of_roads];
        file.close();
        file.open(R"(C:\Users\Hammad\Desktop\Sem3\DSA_Project\road_network.csv)",ios::in);
        if(!file.is_open()) {
            cout<<"File could not be opened"<<endl;
            return;
        }
        string atr[3];
        int i = 0;
        getline(file,line);

        while(getline(file,line)) {
            stringstream ss(line);
            string word;
            getline(ss, word, ',');
            atr[0] = word;
            getline(ss, word, ',');
            atr[1] = word;
            getline(ss, word, ',');
            atr[2] = word;

            roads[i].from = atr[0][0] - 65;
            roads[i].to = atr[1][0] - 65;
            roads[i].weight = stoi(atr[2]);
            i++;
        }
        file.close();
    }
    void makeIntersections(const sf::Font& font) {

        char uniqueNodes[26];
        int uniqueCount = 0;

        for(int i = 0; i < 26; ++i) {
            uniqueNodes[i] = '#';
        }
        // Collect unique nodes
        for(int i = 0; i < number_of_roads; ++i) {
            char fromNode = static_cast<char>(roads[i].from + 'A');
            char toNode = static_cast<char>(roads[i].to + 'A');

            // Check if 'fromNode' is already in uniqueNodes
            bool fromExists = false;
            for (int j = 0; j < uniqueCount; ++j) {
                if (uniqueNodes[j] == fromNode) {
                    fromExists = true;
                    break;
                }
            }
            if (!fromExists) {
                uniqueNodes[uniqueCount++] = fromNode;
            }

            // Check if 'toNode' is already in uniqueNodes
            bool toExists = false;
            for (int j = 0; j < uniqueCount; ++j) {
                if (uniqueNodes[j] == toNode) {
                    toExists = true;
                    break;
                }
            }
            if (!toExists) {
                uniqueNodes[uniqueCount++] = toNode;
            }
        }

        // Now uniqueNodes contains all unique nodes, and uniqueCount has the total count
        number_of_intersections = uniqueCount;

        // Allocate memory for intersections
        intersections = new Node[number_of_intersections];

        // Initialize each intersection with a random position
        for (int i = 0; i < number_of_intersections; ++i) {
            float x = static_cast<float>(rand() % 800 + 150);
            float y = static_cast<float>(rand() % 800 + 150);
            intersections[i] = Node(x, y, uniqueNodes[i], font);
        }
    }
    void applyForces() {
        // Repulsion between all nodes
        for (size_t i = 0; i < number_of_intersections; ++i) {
            for (size_t j = i + 1; j < number_of_intersections; ++j) {
                sf::Vector2f delta = intersections[i].position - intersections[j].position;
                float distance = std::max(std::sqrt(delta.x * delta.x + delta.y * delta.y), 8.0f);
                sf::Vector2f repulsion = (REPULSION_FORCE / (distance * distance)) * (delta / distance);

                intersections[i].velocity += repulsion;
                intersections[j].velocity -= repulsion;
            }
        }

        // Attraction along edges
        for (int i = 0; i < number_of_roads; i++) {
            sf::Vector2f delta = intersections[roads[i].from].position - intersections[roads[i].to].position;
            float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            sf::Vector2f attraction = ATTRACTION_FORCE * distance * (delta / distance);

            intersections[roads[i].from].velocity -= attraction;
            intersections[roads[i].to].velocity += attraction;
        }
    }

    ~RoadNetwork() {
        delete[] intersections;
        delete[] roads;
    }
};


//SFML UI
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;

public:
    Button(){}
    Button(float x, float y, float width, float height, const std::string& label, const sf::Color& color,const sf::Font& font) {
        // Set the button shape
        shape.setPosition(x, y);
        shape.setSize({width, height});
        shape.setFillColor(color);

        // Set the button text
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(13); // Adjust as needed
        text.setFillColor(sf::Color::Black);

        // Center the text within the button
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                       textBounds.top + textBounds.height / 2.0f);
        text.setPosition(x + width / 2.0f, y + height / 2.0f);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
        window.draw(text);
    }

    bool isClicked(const sf::Vector2i& mousePos) const {
        return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }

    void setFillColor(const sf::Color& color) {
        shape.setFillColor(color);
    }
};
void displayMenu(sf::RenderWindow& window,Button buttons[],int size) {
    sf::RectangleShape side_panel;
    side_panel.setFillColor(sf::Color(137,168,178));
    side_panel.setPosition(1164,0);
    side_panel.setSize({1600-1164,900});
    window.draw(side_panel);
    for(int i = 0; i < size; ++i) {
        buttons[i].draw(window);
    }
}
void addButtons(Button buttons[],int size,sf::Font& font) {
    string options[] = {"Display City Traffic Network","Display Traffic Signal Status","Display Congestion Status", "Display Block Roads","Handle Emergency Vehicle", "Block Road Due to Accident", "Simulate Vehicle Routing","Exit"};
    for (int i = 0; i < size; ++i) {
        buttons[i] = Button(1164 + 140, 200 + i * 70,170,60,options[i],sf::Color(229,225,218),font);
    }
}

int main() {
    sf::Font font;
    Button buttons[8];
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Dynamic Graph Layout");

    window.setFramerateLimit(60);
    font.loadFromFile(fontPath);
    addButtons(buttons,8,font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color(179,200,207));
        displayMenu(window,buttons,8);
        window.display();
    }

    return 0;
}