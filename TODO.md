# TODO - Corrections du projet webserv

## 1. Corriger le build immédiatement
- Fichier concerné : src/http_handler.cpp
- Problème : ordre des `catch` incorrect, car `http_request::http_error` hérite de `std::runtime_error`.
- Action : placer `catch (const http_request::http_error&)` avant `catch (const std::runtime_error&)`.
- Vérification : `make` doit compiler sans erreur.

## 2. Rendre le parseur de configuration robuste
- Fichiers concernés : src/parse_config.cpp, include/parse_config.hpp, config.conf
- Problème : le parseur actuel est trop fragile et validate peu les directives.
- Actions à faire :
  - valider chaque directive (`port`, `server_name`, `error_page`, `client_max_body_size`, `location`)
  - vérifier la syntaxe des blocs `location`
  - rejeter les config invalides proprement
  - corriger les valeurs incohérentes comme `serv_name = HTML_Example.html;`
- Objectif : transformer la config en un format fiable et exploitable par le serveur.

## 3. Corriger la logique de matching des routes
- Fichiers concernés : include/parse_config.hpp, src/utils.cpp
- Problème : `getOneRoute()` et le routage des chemins sont trop naïfs.
- Actions à faire :
  - choisir la route la plus spécifique possible
  - gérer correctement les under-specified matches (`/`, `/cgi`, `/old_path`)
  - normaliser les chemins avec slashs et racines
- Objectif : éviter les mauvais matches et les chemins incohérents.

## 4. Corriger la gestion des chemins et fichiers
- Fichiers concernés : src/utils.cpp, src/http_handler.cpp, src/parse_config.cpp
- Problème : génération et validation de chemins insuffisantes.
- Actions à faire :
  - normaliser les chemins
  - empêcher les chemins hors de la racine configurée
  - vérifier les permissions et l’existence des fichiers
  - renvoyer les bons codes HTTP (404 / 403)
- Objectif : sécuriser le serving de fichiers et les redirections.

## 5. Sécuriser le CGI
- Fichier concerné : src/cgi_handler.cpp
- Problème : `rebase_to_script_dir()` fait un `chdir()` global, ce qui est dangereux dans un serveur.
- Actions à faire :
  - ne plus modifier le répertoire courant global du serveur
  - gérer le script CGI sans `chdir()`
  - vérifier existence et exécutable du script
  - passer l’environnement CGI proprement
- Objectif : éviter les effets de bord et rendre le CGI fiable.

## 6. Corriger le parsing HTTP et la validation des requêtes
- Fichier concerné : src/http_request.cpp
- Problème : le parseur HTTP est trop minimal et ne valide pas assez les requêtes.
- Actions à faire :
  - valider la request line (`METHOD URL HTTP/version`)
  - contrôler les headers (`Host`, `Content-Length`, `Transfer-Encoding`)
  - rejeter les requêtes mal formées
  - gérer proprement les cas poubelles et incomplets
- Objectif : éviter les requêtes invalides et gérer les erreurs HTTP proprement.

## 7. Corriger la gestion des erreurs HTTP
- Fichiers concernés : src/http_handler.cpp, src/http_response.cpp, include/http_response.hpp
- Problème : réponses d’erreur peu robustes et parfois mal construites.
- Actions à faire :
  - vérifier automatiquement le bon `Content-Type`
  - vérifier `Content-Length`
  - vérifier le message HTTP associé au code
  - gérer les pages d’erreur correctement
- Objectif : produire des réponses HTTP cohérentes et conformes.

## 8. Vérifier les méthodes HTTP et leur logique
- Fichiers concernés : src/http_handler.cpp, include/http_request.hpp
- Problème : GET/POST/DELETE existent, mais la logique n’est pas toujours cohérente.
- Actions à faire :
  - gérer explicitement la cible du fichier et la route
  - vérifier les permissions avant l’écriture/suppression
  - corriger la logique DELETE qui ne doit pas juste tronquer un fichier
  - sécuriser l’écriture de POST
- Objectif : rendre le comportement HTTP conforme à un vrai serveur web.

## 9. Corriger le comportement réseau / epoll
- Fichiers concernés : src/epoll_utils.cpp, include/epoll_utils.hpp
- Problème : la gestion des sockets et des événements est très basique.
- Actions à faire :
  - vérifier chaque fd avant ajout dans epoll
  - gérer propres les événements EPOLLERR / EPOLLHUP / EPOLLRDHUP / EPOLLOUT
  - fermer proprement les connexions
  - éviter les fuites de fd
- Objectif : stabilité du serveur sous charge et en cas d’erreur.

## 10. Corriger les erreurs de configuration exemple
- Fichier concerné : config.conf
- Problème : le fichier de config exemple n’est pas cohérent comme configuration HTTP.
- Actions à faire :
  - remplacer `serv_name = HTML_Example.html;` par un vrai nom de serveur (ex. `localhost`)
  - vérifier toutes les routes et racines existantes
  - harmoniser les chemins de `server_files`
- Objectif : avoir une config qui reflète un serveur web valide et fonctionnel.

## 11. Vérifier les fichiers statiques et ressources
- Dossier concerné : server_files
- Problème : les chemins et les fichiers référencés ne sont pas toujours utilisés de manière fiable.
- Actions à faire :
  - vérifier toutes les ressources référencées dans la config
  - s’assurer que les fichiers existent bien
  - vérifier les liens vers les dossiers et fichiers CGI
- Objectif : éviter les 404 / 403 inutiles et les erreurs de route.

## 12. Améliorer la robustesse générale et le shutdown
- Fichiers concernés : src/main.cpp, src/epoll_utils.cpp
- Problème : le serveur manque de robustesse face aux arrêts et aux erreurs.
- Actions à faire :
  - nettoyer les ressources en sortie
  - gérer plus proprement les signaux et la fin du programme
  - éviter les fds restants ouverts
- Objectif : garantir un comportement stable et propre du serveur.

## Priorité recommandée
1. Build / catch
2. Config parser
3. Routing
4. CGI
5. HTTP parsing et erreurs
6. Epoll / sockets
7. Nettoyage global
