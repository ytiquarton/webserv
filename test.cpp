#include <iostream>
#include <fstream>
#include <vector>
#include "include/parse_config.hpp" // Ajustez le chemin selon votre structure

// Fonction utilitaire pour créer un fichier de configuration de test
void createTestConfigFile(const std::string& filename) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Impossible de créer le fichier de test." << std::endl;
        return;
    }

    file << "# Fichier de configuration de test WebServ\n"
         << "server {\n"
         << "    port = 8080;\n"
         << "    serv_name = localhost;\n"
         << "    error_page = 404 403 /errors/40x.html;\n"
         << "    error_page = 500 502 /errors/50x.html;\n"
         << "    client_max_body_size = 10M;\n"
         << "}\n"
         << "\n"
         << "server {\n"
         << "    listen = 443;\n"
         << "    server_name = example.com;\n"
         << "    client_max_body_size = 500K;\n"
         << "    # Bloc location fictif pour tester la gestion des accolades imbriquées\n"
         << "    location / {\n"
         << "        root = /var/www/html;\n"
         << "    }\n"
         << "}\n";

    file.close();
}

int main() {
    std::string configPath = "test_webserv.conf";

    // 1. Génération du fichier de test
    createTestConfigFile(configPath);

    // 2. Lancement du parsing
    ParseConf parser;
    std::vector<server> servers = parser.parse_server(configPath);

    // 3. Affichage des résultats
    std::cout << "==========================================" << std::endl;
    std::cout << "Nombre de serveurs parses : " << servers.size() << std::endl;
    std::cout << "==========================================" << std::endl;

    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "\n--- Serveur " << (i + 1) << " ---" << std::endl;
        
        // Remplacez ces getters par ceux réellement présents dans votre classe 'server'
        std::cout << "Port                 : " << servers[i].getPort() << std::endl;
        std::cout << "Server Name          : " << servers[i].getServerName() << std::endl;
        std::cout << "Client Max Body Size : " << servers[i].getClientMaxBodySize() << " octets" << std::endl;

        // Si votre classe server expose un getter pour les pages d'erreur (ex: map<int, string>)
        /*
        std::map<int, std::string> errors = servers[i].getErrorPages();
        std::cout << "Pages d'erreur :" << std::endl;
        for (std::map<int, std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
            std::cout << "  - Code " << it->first << " -> " << it->second << std::endl;
        }
        */
    }

    return 0;
}