
//
// Mustapha
// TP-1
//

#include "DonneesGTFS.h"
#include <algorithm>
#include <iostream>

using namespace std;


//! \brief ajoute les lignes dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les lignes
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterLignes(const std::string &p_nomFichier) {

    // Ouverture du fichier et lancer une exeption si un echec
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }
    // Pour enlever la premiere ligne
    string premiereLigne;
    getline(ifs, premiereLigne);

    string ligneFichier;
    vector<string> temp;

    while (getline(ifs, ligneFichier))
    {
        // Enlever les "" du string
        ligneFichier.erase(remove(ligneFichier.begin(), ligneFichier.end(), '"'), ligneFichier.end());
        temp = string_to_vector(ligneFichier, ',');

        // Ajouter au m_ligne les cles et leurs valuers
        m_lignes.insert(pair<unsigned int, Ligne>(stoi(temp.at(0)),
                                                  Ligne(stoi(temp.at(0)),temp.at(2),
                                                        temp.at(4),
                                                        Ligne::couleurToCategorie(temp.at(7)))));

        // Ajouter au m_ligne les cles et leurs valuers
        m_lignes_par_numero.insert(pair<string , Ligne>(temp.at(2),
                                                        Ligne(stoi(temp.at(0)),temp.at(2),
                                                              temp.at(4),
                                                              Ligne::couleurToCategorie(temp.at(7)))));

        // supprimer les elements deja utliser.
        temp.clear();
    }

    // Fermer le fichier ouvert
    ifs.close();
}

//! \brief ajoute les stations dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterStations(const std::string &p_nomFichier)
{
    // Ouverture du fichier et lancer une exeption si un echec
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }
    // Pour enlever la premiere ligne
    string premiereLigne;
    getline(ifs, premiereLigne);

    string ligneFichier;
    vector<string> temp;

    while (getline(ifs, ligneFichier))
    {
        // Enlever les "" du string
        ligneFichier.erase(remove(ligneFichier.begin(), ligneFichier.end(), '"'), ligneFichier.end());
        temp = string_to_vector(ligneFichier, ',');

        // Ajouter au m_station les cles et leurs valuers
        m_stations.insert(pair<unsigned int, Station>(stoi(temp.at(0)),
                                                      Station(stoi(temp.at(0)),
                                                              temp.at(1),temp.at(2),
                                                              Coordonnees(stod(temp.at(3)),
                                                                          stod(temp.at(4))))));

        // supprimer les elements deja utliser.
        temp.clear();
    }

    // Fermer le fichier ouvert
    ifs.close();
}



//! \brief ajoute les transferts dans l'objet GTFS
//! \breif Cette méthode doit âtre utilisée uniquement après que tous les arrêts ont été ajoutés
//! \brief les transferts (entre stations) ajoutés sont uniquement ceux pour lesquelles les stations sont prensentes dans l'objet GTFS
//! \brief les transferts sont ajoutés dans m_transferts
//! \brief les from_station_id des stations de transfert sont ajoutés dans m_stationsDeTransfert
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
//! \throws logic_error si tous les arrets de la date et de l'intervalle n'ont pas été ajoutés
void DonneesGTFS::ajouterTransferts(const std::string &p_nomFichier)
{
    // Ouverture du fichier et lancer une exeption si un echec
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs)
    {
        throw logic_error("Le fichier n'existe pas");
    }

    // lancer une exeption si pas tout les arrets existe
    if(!m_tousLesArretsPresents)
        throw logic_error("Le fichier n'existe pas");

    // Pour enlever la premiere ligne
    string premiereLigne;
    getline(ifs, premiereLigne);

    string ligneFichier;
    vector<string> temp;

    while (getline(ifs, ligneFichier))
    {
        // Enlever les "" du string
        ligneFichier.erase(remove(ligneFichier.begin(), ligneFichier.end(), '"'), ligneFichier.end());
        temp = string_to_vector(ligneFichier, ',');

        // Changer 0 en 1 et conversion en int
        int sec;
        {
            if (stoi(temp.at(3)) == 0)
                sec = 1;
            else
                sec = stoi(temp.at(3));
        }

        // Verifier si tous les arrets sont presents
        if(m_tousLesArretsPresents)
        {
            if(m_stations.find(stoi(temp.at(0))) != m_stations.end() &&
                    m_stations.find(stoi(temp.at(1))) != m_stations.end())
            {
                m_transferts.emplace_back(make_tuple(stoi(temp.at(0)),stoi(temp.at(1)), sec));
                m_stationsDeTransfert.insert(stoi(temp.at(0)));
            }
        }

        // supprimer les elements deja utliser.
        temp.clear();
    }

    // Fermer le fichier ouvert
    ifs.close();

}


//! \brief ajoute les services de la date du GTFS (m_date)
//! \param[in] p_nomFichier: le nom du fichier contenant les services
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterServices(const std::string &p_nomFichier)
{
    // Ouverture du fichier et lancer une exeption si un echec
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }

    // Pour enlever la premiere ligne
    string premiereLigne;
    getline(ifs, premiereLigne);

    string ligneFichier;
    vector<string> temp;

    while (getline(ifs, ligneFichier)) {
        // Enlever les "" du string
        ligneFichier.erase(remove(ligneFichier.begin(), ligneFichier.end(), '"'), ligneFichier.end());
        temp = string_to_vector(ligneFichier, ',');

        // Ajouter au m_service valuers et verifier si la date correspond au m_date et que exception == "1"
        if (Date(stoi(temp.at(1).substr(0, 4)),
                 stoi(temp.at(1).substr(4, 2)),
                 stoi(temp.at(1).substr(6, 2))) == m_date & temp.at(2) == "1") {
            m_services.insert(temp.at(0));

        }

        // supprimer les elements deja utliser.
        temp.clear();
    }

    // Fermer le fichier ouvert
    ifs.close();
}

//! \brief ajoute les voyages de la date
//! \brief seuls les voyages dont le service est présent dans l'objet GTFS sont ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les voyages
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterVoyagesDeLaDate(const std::string &p_nomFichier)
{

    // Ouverture du fichier et lancer une exeption si un echec
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }

    // Pour enlever la premiere ligne
    string premiereLigne;
    getline(ifs, premiereLigne);
    string ligneFichier;
    vector<string> temp;

    while (getline(ifs, ligneFichier)) {

        // Enlever les "" du string
        ligneFichier.erase(remove(ligneFichier.begin(), ligneFichier.end(), '"'), ligneFichier.end());
        temp = string_to_vector(ligneFichier, ',');



        if(m_services.find(temp.at(1)) != m_services.end())
        {
            m_voyages.insert(pair<string, Voyage>(temp.at(2) ,
                                                  Voyage(temp.at(2),
                                                         stoi(temp.at(0)), temp.at(1),
                                                         temp.at(3))));
        }

        // supprimer les elements deja utliser
        temp.clear();
    }

    // Fermer le fichier ouvert
    ifs.close();
}

//! \brief ajoute les arrets aux voyages présents dans le GTFS si l'heure du voyage appartient à l'intervalle de temps du GTFS
//! \brief Un arrêt est ajouté SSI son heure de départ est >= now1 et que son heure d'arrivée est < now2
//! \brief De plus, on enlève les voyages qui n'ont pas d'arrêts dans l'intervalle de temps du GTFS
//! \brief De plus, on enlève les stations qui n'ont pas d'arrets dans l'intervalle de temps du GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les arrets
//! \post assigne m_tousLesArretsPresents à true
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterArretsDesVoyagesDeLaDate(const std::string &p_nomFichier)
{
    // Ouverture du fichier et lancer une exeption si un echec
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }

    // Pour enlever la premiere ligne
    string premiereLigne;
    getline(ifs, premiereLigne);

    string ligneFichier;
    vector<string> temp;

    while (getline(ifs, ligneFichier)) {

        // Enlever les "" du string
        ligneFichier.erase(remove(ligneFichier.begin(), ligneFichier.end(), '"'), ligneFichier.end());
        temp = string_to_vector(ligneFichier, ',');

        Heure debut = Heure(stoi(temp.at(1).substr(0, 2)),
                            stoi(temp.at(1).substr(3, 2)),
                            stoi(temp.at(1).substr(6, 2)));

        Heure fin = Heure(stoi(temp.at(2).substr(0, 2)),
                          stoi(temp.at(2).substr(3, 2)),
                          stoi(temp.at(2).substr(6, 2)));

        // Trier selon les heures
        if (m_voyages.find(temp.at(0)) != m_voyages.end() && m_now1 <= debut && m_now2 > fin) {
            Arret::Ptr a_ptr = make_shared<Arret>(stoi(temp.at(3)), debut, fin,
                                                  stoi(temp.at(4)), temp.at(0));

            // Ajout des arret a Voyage
            m_voyages[temp.at(0)].ajouterArret(a_ptr);

            // Ajout des arret a Station
            m_stations[a_ptr->getStationId()].addArret(a_ptr);

            m_nbArrets += 1;
        }

        // supprimer les elements deja utliser
        temp.clear();
    }

    // Supprimer Voyages qui n'ont pas d'arret
    map<string, Voyage>::iterator i;
    i = m_voyages.begin();
    while (i != m_voyages.end())
    {
        if(i->second.getArrets().empty())
        {
            m_voyages.erase(i->first);
            i = m_voyages.begin();
        }
        else
            i++;
    }

    //Supprimer Stations qui n'ont pas d'arret
    map<unsigned int, Station>::iterator j;
    j = m_stations.begin();
    while (j != m_stations.end())
    {
        if(j->second.getArrets().empty())
        {
            m_stations.erase(j->first);
            j = m_stations.begin();
        }
        else
            j++;
    }

    // Modifier m_touLesArretsPresents
    m_tousLesArretsPresents = true;

    // Fermer le fichier ouvert
    ifs.close();
}



