#include<iostream>
#include<fstream>
#include<sstream>
#include<cmath>
#include<chrono>
#include<SFML/Graphics.hpp>
#define INF INT_MAX
using namespace std;

//Constant Macros And Global Variables
constexpr float REPULSION_FORCE = 5000.0f;
constexpr float DAMPING = 0.85f;
constexpr float ATTRACTION_FORCE = 0.01f;
const string fontPath = R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\Arial.ttf)";
int number_of_intersections = 0;
int number_of_roads = 0;
int Adj_Matrix[26][26];
int congestion_matrix[26][26]{0};
auto Clock = std::chrono::steady_clock::now();

//Function Declarations
string dijkstra(char,char);

//Road Network and Graph
class RoadNetwork {
    void place_sprites() {
        for(int i = 0; i < number_of_intersections; i++) {
            sprites[i].setPosition(intersections[i].position);
            sprites[i].setTexture(texture[0]);
            sprites[i].setColor(sf::Color(255,255,255,255));
        }
    }
    public:
    RoadNetwork() {
        intersections = nullptr;
        roads = nullptr;
        texture[0].loadFromFile(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\green_light.png)");
        texture[1].loadFromFile(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\red_light.png)");
        if (!texture[0].loadFromFile(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\green_light.png)")) {
            cerr << "Error: Could not load image.png" << endl;
        }
        //cout<<texture[0].getSize().x<<" "<<texture[0].getSize().y<<endl;
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
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::Red);

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
        int congestion;
        int weight;
        string status = " ";
        bool isBlocked = false;

        Edge() {
            from = 0;
            to = 0;
            weight = 0;
            congestion = 0;
        }
        Edge(const unsigned char from, const unsigned char to, const float weight) {
            this->from = from - 65;
            this->to = to - 65;
            this->weight = weight;
            congestion = 0;
        }
        void print()const {
            cout << "from: " << from << " to: " << to << " weight: " << weight << endl;
        }

    }*roads;
    int light_time[26];
    sf::Sprite sprites[26];
    sf::Texture texture[2];
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
        file.open(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\road_network.csv)",ios::in);
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
            //cout<<atr[0][0]<<' '<<atr[1][0]<<' '<<atr[2]<<endl;
            i++;
        }
        file.close();

        file.open(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\road_closures.csv)",ios::in);

        if(!file.is_open()) {
            cout<<"File could not be opened"<<endl;
            return;
        }
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

            for(int i = 0; i < number_of_roads; i++) {
                if(atr[0][0]-65 == roads[i].from && atr[1][0]-65 == roads[i].to) {
                    if(atr[2] == "Blocked")
                        roads[i].isBlocked = true;

                    roads[i].status = atr[2];
                }
            }
        }
        file.close();
        file.open(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\traffic_signals.csv)",ios::in);
        i = 0;
        getline(file,line);
        while(getline(file,line)) {
            stringstream ss(line);
            string word;
            getline(ss, word, ',');
            getline(ss, word, ',');
            light_time[i] = stoi(word);
            i++;
        }
        file.close();
        place_sprites();
    }
    void makeIntersections(const sf::Font& font) {

        char uniqueNodes[26];
        int uniqueCount = 0;

        for(char & uniqueNode : uniqueNodes) {
            uniqueNode = '#';
        }
        // Collect unique nodes
        if(roads != nullptr) {
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
                    uniqueNodes[roads[i].from] = fromNode;
                    uniqueCount++;
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
                    uniqueNodes[roads[i].to] = toNode;
                    uniqueCount++;
                }
            }
        }

        // Now uniqueNodes contains all unique nodes, and uniqueCount has the total count
        number_of_intersections = uniqueCount;

        // Allocate memory for intersections
        intersections = new Node[number_of_intersections];

        // Initialize each intersection with a random position
        for (int i = 0; i < 26; ++i) {
            if(uniqueNodes[i] == '#') {
                continue;;
            }
            const auto x = static_cast<float>(rand() % 600 + 150);
            const auto y = static_cast<float>(rand() % 600 + 150);
            intersections[i] = Node(x, y, uniqueNodes[i], font);
        }
    }
    void applyForces() {
        // Repulsion between all nodes
        for (size_t i = 0; i < number_of_intersections; ++i) {
            for (size_t j = i + 1; j < number_of_intersections; ++j) {
                sf::Vector2f delta = intersections[i].position - intersections[j].position;
                const float distance = std::max(std::sqrt(delta.x * delta.x + delta.y * delta.y), 8.0f);
                const sf::Vector2f repulsion = (REPULSION_FORCE / (distance * distance)) * (delta / distance);

                intersections[i].velocity += repulsion;
                intersections[j].velocity -= repulsion;
            }
        }

        // Attraction along edges
        if(roads) {
            for (int i = 0; i < number_of_roads; i++) {
                sf::Vector2f delta = intersections[roads[i].from].position - intersections[roads[i].to].position;
                const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                const sf::Vector2f attraction = ATTRACTION_FORCE * distance * (delta / distance);

                intersections[roads[i].from].velocity -= attraction;
                intersections[roads[i].to].velocity += attraction;
            }
        }

        //update_sprites();
    }
    void showTrafficLights(sf::RenderWindow& window) const {
        for(int i = 0; i < number_of_intersections; i++) {
            window.draw(sprites[i]);
            //cout<<sprites[i].getPosition().x<<' '<<sprites[i].getPosition().y<<endl;
        }
    }
    void update_sprites() {
        for(int i = 0; i < number_of_intersections; i++) {
            sprites[i].setPosition(intersections[i].position);
        }
    }

    ~RoadNetwork() {
        delete[] intersections;
        delete[] roads;
    }
};

//Vehicle
class Vehicles {
public:
    struct Vehicle {
        bool isEmergency;
        char source;
        char destination;
        string path;
        int steps = 0;
        int current_time_on_path = 0;
        bool reached = false;
    };
    Vehicle* vehicles;
    int vehicleCount;

    explicit Vehicles(const RoadNetwork& Road) {
        vehicles = nullptr;
        vehicleCount = 0;
        fstream file(R"(C:\Users\Hammad\Desktop\DSA_Traffic_Management_Project\vehicles.csv)",ios::in);
        if(!file.is_open()) {
            cout<<"File could not be opened"<<endl;
        }
        else {
            string line;
            getline(file,line);
            while(getline(file,line)) {
                vehicleCount++;
            }
            vehicles = new Vehicle[vehicleCount];
            file.clear();
            file.seekg(0,ios::beg);
            getline(file,line);
            int i = 0;
            while(getline(file,line)) {
                stringstream ss(line);
                string word;
                getline(ss, word, ',');
                getline(ss, word, ',');
                vehicles[i].source = word[0];
                getline(ss, word, ',');
                vehicles[i].destination = word[0];
                vehicles[i].path = dijkstra(vehicles[i].source,vehicles[i].destination);
                i++;
            }
        }
    }

    ~Vehicles() {
        delete [] vehicles;
    }
};


//SFML UI AND DISPLAYS
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;

public:
    bool buttonClicked = false;
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

    void resetFillColor() {
        shape.setFillColor(sf::Color(229,225,218));
    }
};
void displayGraph(sf::RenderWindow& window,RoadNetwork& Road,sf::Font& font) {
        Road.applyForces();
        for(int i = 0; i < number_of_roads; ++i) {
            sf::Vector2f tempPos = Road.intersections[i].position;
            tempPos += Road.intersections[i].velocity;
            if(tempPos.x > 60 && tempPos.x <= 900 && tempPos.y > 60 && tempPos.y <= 840) {
                Road.intersections[i].update();
            }
        }

        // Draw edges
        for (int i = 0; i < number_of_roads; ++i) {
            sf::Text weight;
            weight.setCharacterSize(10);
            weight.setFont(font);
            weight.setFillColor(sf::Color::Black);
            sf::Vertex line[] = {
                sf::Vertex(Road.intersections[Road.roads[i].from].position, sf::Color::White),
                sf::Vertex(Road.intersections[Road.roads[i].to].position, sf::Color::White)
            };
            float text_x = (line[0].position.x + line[1].position.x)/2;
            float text_y = (line[0].position.y + line[1].position.y)/2;
            string w = to_string(static_cast<int>(Road.roads[i].weight));
            weight.setString(w);
            weight.setPosition(text_x,text_y);

            //if(Road.roads[i].from == Road.roads[index - 65].from && Road.roads[i].to == Road.roads[index - 65].to)
            window.draw(line, 2, sf::Lines);
            window.draw(weight);
        }

        // Draw nodes
        for (int i = 0; i < number_of_intersections; ++i) {
            //window.draw(Road.intersections[i].shape);
                window.draw(Road.intersections[i].text);
        }
}
void displayBlocked(sf::RenderWindow& window,RoadNetwork& Road,sf::Font& font) {
    // Draw edges
    for (int i = 0; i < number_of_roads; ++i) {
        sf::Text weight;
        weight.setCharacterSize(10);
        weight.setFont(font);
        weight.setFillColor(sf::Color::Black);
        sf::Vertex line[] = {
            sf::Vertex(Road.intersections[Road.roads[i].from].position, Road.roads[i].isBlocked? sf::Color::Red:sf::Color::White),
            sf::Vertex(Road.intersections[Road.roads[i].to].position, Road.roads[i].isBlocked? sf::Color::Red:sf::Color::White)
        };
        float text_x = (line[0].position.x + line[1].position.x)/2;
        float text_y = (line[0].position.y + line[1].position.y)/2;
        string w = (Road.roads[i].status);
        weight.setString(w);
        weight.setPosition(text_x,text_y);
        window.draw(line, 2, sf::Lines);
        window.draw(weight);
    }

    // Draw nodes
    for (int i = 0; i < number_of_intersections; ++i) {
        //window.draw(intersections[i].shape);
        window.draw(Road.intersections[i].text);
    }
}
void displayCongestion(sf::RenderWindow& window,RoadNetwork& Road,sf::Font& font) {
    for (int i = 0; i < number_of_roads; ++i) {
        sf::Text weight;
        sf::Color color[] = {sf::Color::White,sf::Color::Green,sf::Color::Yellow,sf::Color::Red};
        weight.setCharacterSize(10);
        weight.setFont(font);
        weight.setFillColor(sf::Color::Black);
        sf::Vertex line[] = {
            sf::Vertex(Road.intersections[Road.roads[i].from].position,color[congestion_matrix[Road.roads[i].from][Road.roads[i].to]]),
            sf::Vertex(Road.intersections[Road.roads[i].to].position,color[congestion_matrix[Road.roads[i].from][Road.roads[i].to]])
        };
        float text_x = (line[0].position.x + line[1].position.x)/2;
        float text_y = (line[0].position.y + line[1].position.y)/2;
        string w = to_string(congestion_matrix[Road.roads[i].from][Road.roads[i].to]);
        weight.setString(w);
        weight.setPosition(text_x,text_y);
        window.draw(line, 2, sf::Lines);
        window.draw(weight);
    }

    // Draw nodes
    for (int i = 0; i < number_of_intersections; ++i) {
        //window.draw(intersections[i].shape);
        window.draw(Road.intersections[i].text);
    }
}
void displaySelectedRoute(sf::RenderWindow& window,RoadNetwork& Road,sf::Font& font,const string& path) {

    // Draw edges
    for (int i = 0; i < number_of_roads; ++i) {
        sf::Text weight;
        weight.setCharacterSize(10);
        weight.setFont(font);
        weight.setFillColor(sf::Color::Black);
        sf::Vertex line[] = {
            sf::Vertex(Road.intersections[Road.roads[i].from].position, sf::Color::White),
            sf::Vertex(Road.intersections[Road.roads[i].to].position, sf::Color::White)
        };
        float text_x = (line[0].position.x + line[1].position.x)/2;
        float text_y = (line[0].position.y + line[1].position.y)/2;
        string w = to_string(static_cast<int>(Road.roads[i].weight));
        weight.setString(w);
        weight.setPosition(text_x,text_y);
        //window.draw(line, 2,sf::Lines);

        for(int j = 0; j < path.size()-1; ++j) {
            if(Road.roads[i].from == path[j] - 65 && Road.roads[i].to == path[j+1] - 65) {
                window.draw(line, 2, sf::Lines);
                window.draw(weight);
            }
        }
    }

    // Draw nodes
    for (int i = 0; i < number_of_intersections; ++i) {
        //window.draw(Road.intersections[i].shape);
        window.draw(Road.intersections[i].text);
    }
}
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

//Util Function
void makeAdjMat(const RoadNetwork& Road) {
    for(auto & i : Adj_Matrix) {
        for(int & j : i) {
            j = INF;
        }
    }
    if(Road.roads) {
        for(int i = 0; i < number_of_roads; ++i) {
            Adj_Matrix[Road.roads[i].from][Road.roads[i].to] = static_cast<int>(Road.roads[i].weight);
            Adj_Matrix[i][i] = 0;
            if(Road.roads[i].isBlocked) {
                Adj_Matrix[Road.roads[i].from][Road.roads[i].to] = INF;
            }
        }
    }
}
void makeConMat(const RoadNetwork& Road,const Vehicles& vehicles) {
    for(int i = 0; i < vehicles.vehicleCount; i++) {
        string path = vehicles.vehicles[i].path;
        if(path != "No path found") {
            congestion_matrix[path[0] - 65][path[1] - 65]++;
        }
    }
}
void updateConMat(const RoadNetwork& Road,const Vehicles& vehicles) {
    for(int i = 0 ; i < vehicles.vehicleCount; i++) {
        if(vehicles.vehicles[i].reached) {
            continue;
        }
        int& step = vehicles.vehicles[i].steps;
        int& current_time = vehicles.vehicles[i].current_time_on_path;
        string path = vehicles.vehicles[i].path;   //ABDF
        char source = path[step];
        char destination = path[step + 1];
        if(path != "No path found") {
            if(destination == 0) {
                vehicles.vehicles[i].reached = true;
            }
            current_time++;
            if(current_time == Adj_Matrix[source - 65][destination - 65]) {
                current_time = 0;
                congestion_matrix[source - 65][destination - 65] = congestion_matrix[source - 65][destination - 65] - 1;
                step++;
                source = path[step];
                destination = path[step + 1];
                congestion_matrix[source - 65][destination - 65] = congestion_matrix[source - 65][destination - 65] + 1;
            }
        }
    }
}

//Dijkstra's Algo
int minDistance(const int dist[],const bool visited[],const int number_of_intersections) {
    int min = INF, minIndex = -1;
    for (int v = 0; v < number_of_intersections; v++) {
        if (!visited[v] && dist[v] <= min) {
            min = dist[v];
            minIndex = v;
        }
    }

    return minIndex;
}
string dijkstra(const char source, const char destination) {
    int dist[number_of_intersections] = {INF};
    bool visited[number_of_intersections] = {false};
    int parent[number_of_intersections] = {-1};

    for(int i = 0; i < number_of_intersections; ++i) {
        parent[i] = -1;
        visited[i] = false;
        dist[i] = INF;
    }

    const int src = source - 'A';                // Convert source to index
    const int dest = destination - 'A';          // Convert destination to index
    dist[src] = 0;                         // Distance from source to itself is 0

    for (int count = 0; count < number_of_intersections - 1; count++) {
        int u = minDistance(dist, visited, number_of_intersections); // Pick the minimum distance vertex
        if (u == -1) break; // All remaining number_of_intersections are unreachable
        visited[u] = true;

        // Update distances for neighbors of the picked vertex
        for (int v = 0; v < number_of_intersections; v++) {
            if (!visited[v] && Adj_Matrix[u][v] != INF && dist[u] + Adj_Matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + Adj_Matrix[u][v];
                parent[v] = u; // Update parent for path reconstruction
            }
        }
    }

    // Reconstruct the shortest path
    if (dist[dest] == INF) {
        return "No path found"; // Destination is unreachable
    }

    string path = "";
    for (int v = dest; v != -1; v = parent[v]) {
        path = static_cast<char>(v + 'A') + path; // Build path in reverse
    }

    return path; // Return the path as a string
}


int main() {
    int choice = -1;
    bool simflag = false;
    bool isSecondWindow = false;
    string routing_path,src_dest;
    RoadNetwork Road;
    sf::Font font;
    Button buttons[8];
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Dynamic Graph Layout");
    //sf::RenderWindow* mini_window = nullptr;

    Road.loadRoadData();
    Road.makeIntersections(font);

    window.setFramerateLimit(60);
    font.loadFromFile(fontPath);
    addButtons(buttons,8,font);

    makeAdjMat(Road);
    Vehicles vehicles(Road);
    makeConMat(Road,vehicles);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for(int i = 0; i < 8; i++) {
                    if(buttons[i].isClicked(mousePos)) {
                        buttons[i].setFillColor(sf::Color(229-30,225-30,218-30));
                        buttons[i].buttonClicked = true;
                        choice = i;
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                for(int i = 0; i < 8; i++) {
                    if(buttons[i].buttonClicked) {
                        buttons[i].resetFillColor();
                        buttons[i].buttonClicked = false;
                    }
                }
            }


        }
        Road.update_sprites();

        if(simflag) {
            auto duration = std::chrono::seconds(1);
            auto elapsed_time = std::chrono::steady_clock::now() - Clock;
            if(elapsed_time >= duration) {
                updateConMat(Road,vehicles);
                Clock = std::chrono::steady_clock::now();
            }
        }

        if (window.isOpen()) {
            displayMenu(window,buttons,8);
            window.display();
            window.clear(sf::Color(179,200,207));
            switch (choice) {
                case 0:
                    if(Road.roads)displayGraph(window,Road,font);
                break;

                case 1:
                    Road.showTrafficLights(window);
                    break;

                case 2:
                    displayCongestion(window,Road,font);
                    simflag = true;
                    break;

                case 3:
                    displayBlocked(window,Road,font);
                    break;

                case 5:
                    for(int i = 0 ; i < 26; i++) {
                        for(int j = 0; j < 26; j++) {
                            if(Adj_Matrix[i][j] == INF) {
                                cout<<"I ";
                            }
                            else {
                                cout<<Adj_Matrix[i][j]<<' ';
                            }
                        }
                        cout<<endl;
                    }
                    choice = -1;
                    break;

                case 6: {
                    // Open the mini window
                    sf::RenderWindow mini_window(sf::VideoMode(400, 400), "Vehicle Routing");

                    // Text object to display user input
                    sf::Text inputText;
                    inputText.setFont(font);
                    inputText.setCharacterSize(24);
                    inputText.setFillColor(sf::Color::Black);
                    inputText.setPosition(10.f, 10.f);

                    // String to store the input
                    string inputString;

                    while (mini_window.isOpen()) {
                        sf::Event mini_event;
                        while (mini_window.pollEvent(mini_event)) {
                            if (mini_event.type == sf::Event::Closed) {
                                mini_window.close();
                            }

                            if(inputString.size() ==  2) {
                                mini_window.close();
                                src_dest = inputString;
                            }

                            // Handle text input
                            if (mini_event.type == sf::Event::TextEntered) {
                                char enteredChar = static_cast<char>(mini_event.text.unicode);

                                if (enteredChar == 8) {
                                    if (!inputString.empty()) {
                                        inputString.pop_back();
                                    }
                                } else if (enteredChar >= 65 && enteredChar <= 65+26) { // Handle printable characters
                                    inputString += enteredChar;
                                }

                                inputText.setString(inputString); // Update the displayed text
                            }
                        }

                        mini_window.clear(sf::Color(220, 220, 220)); // Background for mini window
                        mini_window.draw(inputText);                // Draw the text
                        mini_window.display();
                    }

                    choice = 10; // Reset choice after the mini window is closed
                } break;

                case 10:
                    routing_path = dijkstra(src_dest[0],src_dest[1]);
                    if(routing_path != "No path found") {
                        displaySelectedRoute(window,Road,font,routing_path);
                    }
                    else
                        choice = 0;
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}