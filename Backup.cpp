bool sim_flag = true;
    RoadNetwork Road;
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Dynamic Graph Layout");
    window.setFramerateLimit(60);
    Road.loadRoadData();
    sf::Font font;
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }
    Button myButton(1000,500,100,40,"myButton",sf::Color::White,font);
    Road.makeIntersections(font);
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 10); // Position in the top-left corner

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (myButton.isClicked(mousePos)) {
                    myButton.setFillColor(sf::Color::Green); // Change color on click
                    if(sim_flag) {
                        sim_flag = false;
                    }
                    else {
                        sim_flag = true;
                    }
                }
            }
        }
        window.clear(sf::Color(179,200,207));
        displayMenu(window);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Update the text to show mouse coordinates
        std::ostringstream oss;
        oss << "Mouse Position: (" << mousePos.x << ", " << mousePos.y << ")";
        text.setString(oss.str());
        window.draw(text);

        myButton.draw(window);
        if(sim_flag) {
            Road.applyForces();
            for(int i = 0; i < number_of_roads; ++i) {
                if(Road.intersections[i].symbol == 'A' || Road.intersections[i].symbol == 'Z' || Road.intersections[i].symbol == 'R') {
                    continue;
                }
                Road.intersections[i].update();
            }

            // Draw edges
            for (int i = 0; i < number_of_roads; ++i) {
                sf::Vertex line[] = {
                    sf::Vertex(Road.intersections[Road.roads[i].from].position, sf::Color::White),
                    sf::Vertex(Road.intersections[Road.roads[i].to].position, sf::Color::White)
                };
                window.draw(line, 2, sf::Lines);
            }

            // Draw nodes
            for (int i = 0; i < number_of_intersections; ++i) {
                //window.draw(intersections[i].shape);
                window.draw(Road.intersections[i].text);
            }
        }

        // Display the rendered frame
        window.display();
    }
    return 0;