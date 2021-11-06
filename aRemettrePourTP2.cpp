//
// Created by Mario Marchand on 16-12-30.
//

#include "ReseauGTFS.h"
#include <sys/time.h>

using namespace std;

//! \brief ajout des arcs dus aux voyages
//! \brief insère les arrêts (associés aux sommets) dans m_arretDuSommet et m_sommetDeArret
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsVoyages(const DonneesGTFS &p_gtfs) {
    if (m_arretDuSommet.size() != 0)
        throw logic_error("Le graphe est deja remplie");

    size_t taille = m_arretDuSommet.size();

    auto voyage = p_gtfs.getVoyages();

    for (auto i = voyage.begin(); i != voyage.end(); i++) {
        auto arret = i->second.getArrets();
        for (auto j = arret.begin(); j != arret.end(); j++) {
            m_sommetDeArret.insert({*j, taille});
            m_arretDuSommet.push_back(*j);
            ++taille;
        }

        for (auto j = arret.begin(); j != arret.end(); j++) {
            if (j != arret.begin()) {
                auto temp = j;
                --temp;
                size_t precedent = m_sommetDeArret[*temp];
                size_t prochain = m_sommetDeArret[*j];
                int arc = (*j)->getHeureArrivee() - (*temp)->getHeureArrivee();
                m_leGraphe.ajouterArc(precedent, prochain, arc);
            }
        }
    }
}


//! \brief ajouts des arcs dus aux transferts entre stations
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsTransferts(const DonneesGTFS &p_gtfs) {
    if (m_arretDuSommet.size() == 0)
        throw logic_error("Le graphe est deja remplie");

    map<Arret::Ptr, vector<Arret::Ptr>> conteneur_temp;
    vector<Arret::Ptr> vecteur_temp_to;
    vector<string> vecteur_ligne;
    for (auto i = p_gtfs.getTransferts().begin(); i != p_gtfs.getTransferts().end(); i++) {
        int from = get<0>(*i);
        int to = get<1>(*i);
        int minTemps = get<2>(*i);

        auto from_arr = p_gtfs.getStations().find(from)->second.getArrets();
        auto to_arr = p_gtfs.getStations().find(to)->second.getArrets();

        for (auto j = from_arr.begin(); j != from_arr.end(); j++) {
            Heure heure = j->first;
            auto temp_to_1 = to_arr.lower_bound(heure.add_secondes(minTemps));
            auto ligne_from = p_gtfs.getLignes().find(
                    p_gtfs.getVoyages().find(j->second->getVoyageId())->second.getLigne())->second.getNumero();

            for (auto k = temp_to_1; k != to_arr.end(); k++) {
                bool present = false;
                auto ligne_to = p_gtfs.getLignes().find(
                        p_gtfs.getVoyages().find(k->second->getVoyageId())->second.getLigne())->second.getNumero();

                for (auto l = vecteur_ligne.begin(); l != vecteur_ligne.end(); l++) {
                    if (ligne_to == *l)
                        present = true;
                }

                if (!present & (ligne_to != ligne_from)) {
                    vecteur_temp_to.push_back((*k).second);
                    vecteur_ligne.push_back(ligne_to);
                }
            }
            conteneur_temp.insert({(*j).second, vecteur_temp_to});
            vecteur_temp_to.clear();
            vecteur_ligne.clear();
        }

        for (auto i = conteneur_temp.begin(); i != conteneur_temp.end(); i++) {
            auto from = m_sommetDeArret[(*i).first];

            for (auto j = i->second.begin(); j != i->second.end(); j++) {
                auto to = m_sommetDeArret[(*j)];
                auto arc = (*j)->getHeureArrivee() - (*i).first->getHeureArrivee();
                m_leGraphe.ajouterArc(from, to, arc);
            }
        }
        conteneur_temp.clear();
    }
}

//! \brief ajouts des arcs d'une station à elle-même pour les stations qui ne sont pas dans DonneesGTFS::m_stationsDeTransfert
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsAttente(const DonneesGTFS &p_gtfs) {
    if (m_arretDuSommet.size() == 0)
        throw logic_error("Le graphe est deja remplie");

    map<unsigned int, Station> station_sans_transfer;
    for (auto i = p_gtfs.getStations().begin(); i != p_gtfs.getStations().end(); i++) {
        bool present = false;
        for (auto j = p_gtfs.getStationsDeTransfert().begin(); j != p_gtfs.getStationsDeTransfert().end(); j++) {
            if (i->first == *j)
                present = true;

        }

        if (!present)
            station_sans_transfer.insert({i->first, i->second});
    }

    map<Arret::Ptr, vector<Arret::Ptr>> conteneur_temp;
    vector<Arret::Ptr> tempo;
    vector<string> vecteur_ligne;

    for (auto i = station_sans_transfer.begin(); i != station_sans_transfer.end(); i++) {
        for (auto j = i->second.getArrets().begin(); j != i->second.getArrets().end(); j++) {
            auto k = j;
            ++k;
            auto ligne_1 = p_gtfs.getLignes().find(
                    p_gtfs.getVoyages().find(j->second->getVoyageId())->second.getLigne())->second.getNumero();
            for (; k != i->second.getArrets().end(); k++) {
                auto ligne_2 = p_gtfs.getLignes().find(
                        p_gtfs.getVoyages().find(k->second->getVoyageId())->second.getLigne())->second.getNumero();
                bool prensent = false;
                for (auto l = vecteur_ligne.begin(); l != vecteur_ligne.end(); l++) {
                    if ((*l) == ligne_2)
                        prensent = true;
                }
                if (!prensent & (ligne_1 != ligne_2) &
                    ((k->second->getHeureArrivee() - j->second->getHeureArrivee()) >= delaisMinArcsAttente)) {
                    vecteur_ligne.push_back(ligne_2);
                    tempo.push_back((*k).second);
                }
            }
            conteneur_temp.insert({(*j).second, tempo});
            tempo.clear();
            vecteur_ligne.clear();
        }

        for (auto i = conteneur_temp.begin(); i != conteneur_temp.end(); i++) {
            for (auto j = i->second.begin(); j != i->second.end(); j++) {
                auto a = m_sommetDeArret[(*j)];
                auto de = m_sommetDeArret[(*i).first];
                auto arc = (*j)->getHeureArrivee() - (*i).first->getHeureArrivee();
                m_leGraphe.ajouterArc(de, a, arc);
            }
        }
        conteneur_temp.clear();
    }
}


//! \brief ajoute des arcs au réseau GTFS à partir des données GTFS
//! \brief Il s'agit des arcs allant du point origine vers une station si celle-ci est accessible à pieds et des arcs allant d'une station vers le point destination
//! \param[in] p_gtfs: un objet DonneesGTFS
//! \param[in] p_pointOrigine: les coordonnées GPS du point origine
//! \param[in] p_pointDestination: les coordonnées GPS du point destination
//! \throws logic_error si une incohérence est détecté lors de la construction du graphe
//! \post constuit un réseau GTFS représenté par un graphe orienté pondéré avec poids non négatifs
//! \post assigne la variable m_origine_dest_ajoute à true (car les points orignine et destination font parti du graphe)
//! \post insère dans m_sommetsVersDestination les numéros de sommets connctés au point destination
void ReseauGTFS::ajouterArcsOrigineDestination(const DonneesGTFS &p_gtfs, const Coordonnees &p_pointOrigine,
                                               const Coordonnees &p_pointDestination)
{
    if (m_origine_dest_ajoute)
    {
        throw logic_error("Les acrs de l'origine sont deja presents dans le graphe");
    }

    Arret::Ptr arret_origine = make_shared<Arret>(stationIdOrigine,
                                                  Heure(0, 0, 0),
                                                  Heure(0, 0, 0), 0, "Origine");

    Arret::Ptr arret_destination = make_shared<Arret>(stationIdDestination,
                                                      Heure(0, 0, 0),
                                                      Heure(0, 0, 0), 0, "Origine");

    m_leGraphe.resize(m_leGraphe.getNbSommets() + 2);
    m_sommetOrigine = m_leGraphe.getNbSommets() - 2;
    m_sommetDestination = m_leGraphe.getNbSommets() - 1;
    m_nbArcsOrigineVersStations = 0;
    m_nbArcsStationsVersDestination = 0;
    m_sommetDeArret.insert({arret_origine, m_sommetOrigine});
    m_sommetDeArret.insert({arret_destination, m_sommetDestination});
    m_arretDuSommet.push_back(arret_origine);
    m_arretDuSommet.push_back(arret_destination);

    vector<Arret::Ptr> vecteur_arret;
    vector<string> vecteur_ligne;
    for (auto itr_station = p_gtfs.getStations().begin(); itr_station != p_gtfs.getStations().end(); itr_station++)
    {
        if (itr_station->second.getCoords() - p_pointOrigine < distanceMaxMarche)
        {
            unsigned int temp_marche = ((p_pointOrigine - itr_station->second.getCoords()) / vitesseDeMarche) * 3600;
            Heure distance_attente = p_gtfs.getTempsDebut();
            auto arret_disponible = itr_station->second.getArrets().lower_bound(
                    distance_attente.add_secondes(temp_marche));

            for (auto j = arret_disponible; j != itr_station->second.getArrets().end(); j++)
            {
                auto ligne = p_gtfs.getLignes().find(
                        p_gtfs.getVoyages().find(j->second->getVoyageId())->second.getLigne())->second.getNumero();
                bool prensent = false;

                for (auto l = vecteur_ligne.begin(); l != vecteur_ligne.end(); l++)
                {
                    if ((*l) == ligne)
                        prensent = true;
                }
                if (!prensent)
                {
                    vecteur_ligne.push_back(ligne);
                    vecteur_arret.push_back((*j).second);
                }

            }

            for (auto i = vecteur_arret.begin(); i != vecteur_arret.end(); i++)
            {
                unsigned int arc = (*i)->getHeureArrivee() - p_gtfs.getTempsDebut();
                m_leGraphe.ajouterArc(m_sommetOrigine, m_sommetDeArret[*i], arc);
                ++m_nbArcsOrigineVersStations;
            }

            vecteur_arret.clear();
            vecteur_ligne.clear();
        }

        if (itr_station->second.getCoords() - p_pointDestination < distanceMaxMarche)
        {
            for (auto i = itr_station->second.getArrets().begin(); i != itr_station->second.getArrets().end(); i++)
            {
                unsigned int arc = ((p_pointDestination - itr_station->second.getCoords()) / vitesseDeMarche) * 3600;
                m_leGraphe.ajouterArc(m_sommetDeArret[i->second], m_sommetDestination, arc);
                m_sommetsVersDestination.push_back(m_sommetDeArret[i->second]);
                ++m_nbArcsStationsVersDestination;
            }
        }
    }
    m_origine_dest_ajoute = true;

}

//! \brief Remet ReseauGTFS dans l'était qu'il était avant l'exécution de ReseauGTFS::ajouterArcsOrigineDestination()
//! \param[in] p_gtfs: un objet DonneesGTFS
//! \throws logic_error si une incohérence est détecté lors de la modification du graphe
//! \post Enlève de ReaseauGTFS tous les arcs allant du point source vers un arrêt de station et ceux allant d'un arrêt de station vers la destination
//! \post assigne la variable m_origine_dest_ajoute à false (les points orignine et destination sont enlevés du graphe)
//! \post enlève les données de m_sommetsVersDestination
void ReseauGTFS::enleverArcsOrigineDestination()
{
    if (!m_origine_dest_ajoute)
    {
        throw logic_error("Les acrs de l'origine sont deja presents dans le graphe");
    }

    for(auto i = m_sommetsVersDestination.begin(); i != m_sommetsVersDestination.end(); i++)
    {
        m_leGraphe.enleverArc(*i, m_sommetDestination);
    }

    m_leGraphe.resize(m_leGraphe.getNbSommets() - 2);
    m_sommetDeArret.erase(m_arretDuSommet[m_sommetOrigine]);
    m_sommetDeArret.erase(m_arretDuSommet[m_sommetDestination]);
    m_arretDuSommet.pop_back();
    m_arretDuSommet.pop_back();
    m_nbArcsOrigineVersStations = 0;
    m_nbArcsStationsVersDestination = 0;
    m_origine_dest_ajoute = false;

}


