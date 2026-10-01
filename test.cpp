#include "./include/parse_config.hpp"
#include <iostream>
#include <fstream>

// Fonction utilitaire pour créer un fichier de config de test
void create_sample_config(const std::string& filename) {
    std::ofstream file(filename.c_str());
    file << "port = 8080;\n";
    file << "serv_name = webserv_test;\n";
    file << "client_max_body_size = 10M;\n";
    file << "error_page = 404 403 /errors/4xx.html;\n";
    file << "error_page = 500 /errors/500.html;\n";
    file << "\n";
    file << "location = /kapouet; \n{\n";
    file << "    allow_methods = GET POST;\n";
    file << "    root = /tmp/www;\n";
    file << "    autoindex = on;\n";
    file << "    index = index.html;\n";
    file << "    upload_store = /tmp/www/uploads;\n";
    file << "    cgi_pass = .py /usr/bin/python3;\n"; // Tester une mauvaise extension ici (ex: .php) pour déclencher l'exception
    file << "    cgi_dir = /tmp/www/cgi-bin;\n";
    file << "}\n";
    file.close();
}

int main() {
    std::string config_file = "config_test.conf";
    
    // 1. Créer le fichier de configuration de test
    create_sample_config(config_file);

    std::cout << "--- DÉBUT DU PARSING ---\n" << std::endl;

    Server server;

    // 2. Parser le fichier avec capture d'exception
    try {
        server = parse_server(config_file);
    } 
    catch (const std::exception& e) {
        std::cerr << " [EXCEPTION ATTRAPÉE] : " << e.what() << std::endl;
        return 1;
    }

    // 3. Affichage des résultats pour vérification
    std::cout << "Port                : " << server.getPort() << std::endl;
    std::cout << "Server Name         : " << server.getServerName() << std::endl;
    std::cout << "Max Body Size (bytes): " << server.getClientMaxBodySize() << std::endl;

    // Affichage des pages d'erreur
    std::cout << "\nPages d'erreur :" << std::endl;
    std::cout << "  - Code 404 -> " << server.getErrorPagePath(404) << std::endl;
    std::cout << "  - Code 403 -> " << server.getErrorPagePath(403) << std::endl;
    std::cout << "  - Code 500 -> " << server.getErrorPagePath(500) << std::endl;

    // Affichage des routes
    std::cout << "\nRoutes configurées :" << std::endl;
    const std::map<std::string, Route>& routes = server.getRoutes();
    
    for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
        std::cout << "  Route Path    : " << it->first << std::endl;
        std::cout << "    Allow Methods : " << it->second.getAllowMethods() << std::endl;
        std::cout << "    Root          : " << it->second.getRoot() << std::endl;
        std::cout << "    Autoindex     : " << (it->second.getAutoindex() ? "true" : "false") << std::endl;
        std::cout << "    Index         : " << it->second.getIndex() << std::endl;
        std::cout << "    Upload Store  : " << it->second.getUploadStore() << std::endl;
        std::cout << "    CGI Pass      : " << it->second.getCgiPass() << std::endl;
        std::cout << "    CGI Dir       : " << it->second.getCgiDir() << std::endl;
    }

    std::cout << "\n--- PARSING TERMINÉ AVEC SUCCÈS ---" << std::endl;

    return 0;
}