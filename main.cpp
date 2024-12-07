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
int number_of_intersections = 0;
int number_of_roads = 0;


//Road Network and Graph
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


int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Dynamic Graph Layout");
    window.setFramerateLimit(60);
    loadRoadData();
    sf::Font font;
    if (!font.loadFromFile(R"(C:\Users\Hammad\Desktop\Sem3\DSA_Project\Arial.ttf)")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }
    makeIntersections(font);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        applyForces();
        window.clear(sf::Color::Black);

        for(int i = 0; i < number_of_roads; ++i) {
            if(intersections[i].symbol == 'A' || intersections[i].symbol == 'Z' || intersections[i].symbol == 'R') {
                continue;
            }
            intersections[i].update();
        }
        // Draw edges
        for (int i = 0; i < number_of_roads; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(intersections[roads[i].from].position, sf::Color::White),
                sf::Vertex(intersections[roads[i].to].position, sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }

        // Draw nodes
        for (int i = 0; i < number_of_intersections; ++i) {
            //window.draw(intersections[i].shape);
            window.draw(intersections[i].text);
        }

        // Display the rendered frame
        window.display();
    }

    delete[] intersections;
    delete[] roads;
    return 0;
}