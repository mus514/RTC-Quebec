//
// Created by Mario Marchand on 16-12-29.
//

#include "DonneesGTFS.h"
#include <algorithm>
#include <iostream>

using namespace std;


//! \brief ajoute les lignes dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les lignes
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterLignes(const std::string &p_nomFichier) {

    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }
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
                                                        temp.at(4),Ligne::couleurToCategorie(temp.at(7)))));

        // Ajouter au m_ligne les cles et leurs valuers
        m_lignes_par_numero.insert(pair<string , Ligne>(temp.at(2),
                                                        Ligne(stoi(temp.at(0)),temp.at(0),
                                                              temp.at(4),Ligne::couleurToCategorie(temp.at(7)))));

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
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }
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

//écrire votre code ici

}


//! \brief ajoute les services de la date du GTFS (m_date)
//! \param[in] p_nomFichier: le nom du fichier contenant les services
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterServices(const std::string &p_nomFichier)
{
    ifstream ifs(p_nomFichier, ios::in);
    if (!ifs) {
        throw logic_error("Le fichier n'existe pas");
    }
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

//écrire votre code ici

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

//écrire votre code ici

}



