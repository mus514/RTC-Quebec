//
// Created by Mario Marchand on 16-12-30.
//

#include "ReseauGTFS.h"
#include <sys/time.h>

using namespace std;

//! \brief ajout des arcs dus aux voyages
//! \brief insère les arrêts (associés aux sommets) dans m_arretDuSommet et m_sommetDeArret
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsVoyages(const DonneesGTFS & p_gtfs)
{
    if(m_arretDuSommet.size() != 0)
        throw logic_error("Le graphe est deja remplie");

    size_t taille = m_arretDuSommet.size();

    auto voyage = p_gtfs.getVoyages();

    for(auto i = voyage.begin(); i != voyage.end(); i++)
    {
        auto arret = i->second.getArrets();
        for(auto j = arret.begin(); j != arret.end(); j++)
        {
            m_sommetDeArret.insert({*j,taille});
            m_arretDuSommet.push_back(*j);
            ++taille;
        }

        for(auto j = arret.begin(); j != arret.end(); j++)
        {
            if(j != arret.begin())
            {
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
void ReseauGTFS::ajouterArcsTransferts(const DonneesGTFS & p_gtfs)
{
    if(m_arretDuSommet.size() == 0)
        throw logic_error("Le graphe est deja remplie");

    map<Arret::Ptr, vector<Arret::Ptr>> conteneur_temp;
    vector<Arret::Ptr> vecteur_temp_to;
    vector<string> vecteur_ligne;
    for(auto i = p_gtfs.getTransferts().begin(); i != p_gtfs.getTransferts().end(); i++) {
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

//            auto ligne_to = p_gtfs.getLignes().find(
//                    p_gtfs.getVoyages().find(temp_to_1->second->getVoyageId())->second.getLigne())->second.getNumero();
//            if (ligne_from != ligne_to)
//            {
//                vecteur_temp_to.push_back(temp_to_1->second);
//                vecteur_ligne.push_back(ligne_to);
//            }
            temp_to_1;
            for (auto k = temp_to_1; k != to_arr.end(); k++) {
                bool present = false;
                auto ligne_to = p_gtfs.getLignes().find(
                        p_gtfs.getVoyages().find(k->second->getVoyageId())->second.getLigne())->second.getNumero();

                for (auto l = vecteur_ligne.begin(); l != vecteur_ligne.end(); l++)
                {
                    if (ligne_to == *l)
                        present = true;
                }

                if (!present & (ligne_to != ligne_from))
                {
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

//            for(auto k = temp_to_1; k != to_arr.end(); k++)
//            {
//                auto ligne_to = p_gtfs.getLignes().find(p_gtfs.getVoyages().find(k->second->getVoyageId())->second.getLigne())->second.getNumero();
//                if((ligne_from != ligne_to) & (j != k))
//                {
//                    vecteur_temp_to.push_back((*k).second);
//                }
//            }
//
//            if(vecteur_temp_to.size() > 0)
//            {
//                conteneur_temp.insert({(*j).second, vecteur_temp_to});
//                vecteur_temp_to.clear();
//            }
//            vecteur_temp_to.clear();
//
//        }
//
//        for(auto k = conteneur_temp.begin(); k!= conteneur_temp.end(); k++)
//        {
//            for(auto j = k->second.begin(); j!= k->second.end(); j++)
//            {
//                auto itr = j;
//                auto comp1 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find((*j)->getVoyageId())->second.getLigne())->second.getNumero();
//                while (itr != k->second.end())
//                {
//                    if((itr != j))
//                    {
//                        auto comp2 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find((*itr)->getVoyageId())->second.getLigne())->second.getNumero();
//                        if((comp1 == comp2))
//                        {
//                            if(((*itr)->getHeureArrivee() > (*j)->getHeureArrivee()))
//                            {
//                                k->second.erase(itr);
//                                itr =j;
//                            }
//                            else if(((*itr)->getHeureArrivee() <= (*j)->getHeureArrivee()))
//                            {
//                                k->second.erase(j);
//                                //j = k->second.begin();
//                                itr = j;
//
//                            }
//                            else
//                                ++itr;
//
//                        }
//                        else
//                            ++itr;
//                    }
//                    else
//                        ++itr;
//                }
//            }
//        }



}

//! \brief ajouts des arcs d'une station à elle-même pour les stations qui ne sont pas dans DonneesGTFS::m_stationsDeTransfert
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsAttente(const DonneesGTFS & p_gtfs)
{
    if(m_arretDuSommet.size() == 0)
        throw logic_error("Le graphe est deja remplie");

    map<unsigned int, Station> station_sans_transfer;
    for(auto i = p_gtfs.getStations().begin(); i != p_gtfs.getStations().end(); i++)
    {
        bool present = false;
        for(auto j = p_gtfs.getStationsDeTransfert().begin(); j != p_gtfs.getStationsDeTransfert().end(); j++)
        {
            if(i->first == *j)
                present = true;

        }

        if(!present)
            station_sans_transfer.insert({i->first, i->second});
    }

    map<Arret::Ptr, vector<Arret::Ptr>> conteneur_temp;
    vector<Arret::Ptr> tempo;
    vector<string> vecteur_ligne;

    for(auto i = station_sans_transfer.begin(); i != station_sans_transfer.end(); i++)
    {
        for(auto j = i->second.getArrets().begin(); j != i->second.getArrets().end(); j++)
        {
            auto k = j;
            ++k;
            auto ligne_1 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find(j->second->getVoyageId())->second.getLigne())->second.getNumero();
            for(;k != i->second.getArrets().end(); k++)
            {
                auto ligne_2 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find(k->second->getVoyageId())->second.getLigne())->second.getNumero();
                bool prensent = false;
                for(auto l = vecteur_ligne.begin(); l != vecteur_ligne.end(); l++)
                {
                    if((*l) == ligne_2)
                        prensent = true;
                }
                if(!prensent & (ligne_1 != ligne_2) & ((k->second->getHeureArrivee() - j->second->getHeureArrivee()) >= delaisMinArcsAttente))
                {
                    vecteur_ligne.push_back(ligne_2);
                    tempo.push_back((*k).second);
                }
            }
            conteneur_temp.insert({(*j).second, tempo});
            tempo.clear();
            vecteur_ligne.clear();
        }

        for(auto i = conteneur_temp.begin(); i!= conteneur_temp.end(); i++)
        {
            for(auto j = i->second.begin(); j != i->second.end(); j++)
            {
                auto a =m_sommetDeArret[(*j)];
                auto de =m_sommetDeArret[(*i).first];
                auto arc = (*j)->getHeureArrivee() - (*i).first->getHeureArrivee();
                m_leGraphe.ajouterArc(de, a, arc);
            }
        }
        conteneur_temp.clear();


    }




//    for(auto i = station_sans_transfer.begin(); i != station_sans_transfer.end(); i++)
//    {
//        for(auto j = i->second.getArrets().begin(); j != i->second.getArrets().end(); j++)
//        {
//            auto k = j;
//            ++k;
//            auto ligne_1 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find(j->second->getVoyageId())->second.getLigne())->second.getNumero();
//
//            while(k != i->second.getArrets().end())
//            {
//                auto ligne_2 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find(k->second->getVoyageId())->second.getLigne())->second.getNumero();
//
//                if((ligne_1 != ligne_2) & ((k->second->getHeureArrivee() - j->second->getHeureArrivee()) >= delaisMinArcsAttente))
//                {
//                    tempo.push_back(k->second);
//                }
//                ++k;
//            }
//
//            if(tempo.size() > 0)
//            {
//                conteneur_temp.insert({j->second, tempo});
//                tempo.clear();
//            }
//
//            tempo.clear();
//        }

//        for(auto k = conteneur_temp.begin(); k != conteneur_temp.end(); k++)
//        {
//            for(auto j = k->second.begin(); j != k->second.end(); j++)
//            {
//                auto itr = k->second.begin();
//                auto comp2 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find((*j)->getVoyageId())->second.getLigne())->second.getNumero();
//
//                while (itr != k->second.end())
//                {
//                    if((itr != j))
//                    {
//                        auto comp1 = p_gtfs.getLignes().find(p_gtfs.getVoyages().find((*itr)->getVoyageId())->second.getLigne())->second.getNumero();
//
//                        if(comp1 == comp2)
//                        {
//                             Heure heure = k->first->getHeureArrivee();
//                            if((*itr)->getHeureArrivee() > (*j)->getHeureArrivee())
//                            {
//                                k->second.erase(itr);
//                                itr = j;
//                            }
//                            else if((*itr)->getHeureArrivee() <= (*j)->getHeureArrivee())
//                            {
//                                k->second.erase(j);
//                                //j =  k->second.begin();
//                                itr = j;
//                                  //++itr;
//
//                            }
//                            else
//                                ++itr;
//
//                        }
//
//                        else
//                            ++itr;
//                    }
//                    else
//                        ++itr;
//                }
//            }
//        }
//        for(auto i = conteneur_temp.begin(); i!= conteneur_temp.end(); i++)
//        {
//            for(auto j = i->second.begin(); j != i->second.end(); j++)
//            {
//                auto a =m_sommetDeArret[(*j)];
//                auto de =m_sommetDeArret[(*i).first];
//                auto arc = (*j)->getHeureArrivee() - (*i).first->getHeureArrivee();
//                m_leGraphe.ajouterArc(de, a, arc);
//            }
//        }
//        conteneur_temp.clear();
//    }

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
//    Arret::Ptr arret_origine =make_shared<Arret>(stationIdOrigine,
//                                                 Heure(0,0,0),
//                                                 Heure(0,0,0),0,"Origine");
//
//    Arret::Ptr arret_destination =make_shared<Arret>(stationIdDestination,
//                                                 Heure(0,0,0),
//                                                 Heure(0,0,0),0,"Origine");
//
//    m_sommetOrigine = m_arretDuSommet.size();
//    m_sommetDestination = m_arretDuSommet.size() + 1;
//    m_leGraphe.resize(m_leGraphe.getNbSommets() + 2);
//    m_nbArcsOrigineVersStations = 0;
//    m_nbArcsStationsVersDestination = 0;

//    if (m_origine_dest_ajoute == true){
//        throw logic_error("Des arcs d'origine sont déjà présents dans le graphe");
//    }
//
//    const Heure heureDepart = p_gtfs.getTempsDebut();
//    m_nbArcsStationsVersDestination = 0;
//    m_nbArcsOrigineVersStations = 0;
//
//    // On crée des Arrets pointeurs pour l'origine et la destination
//    Arret::Ptr pointOrigine = make_shared<Arret>(stationIdOrigine, heureDepart, Heure(2, 0, 0), 0, "ORIGINE");
//    Arret::Ptr pointDestination = make_shared<Arret>(stationIdDestination, heureDepart, Heure(2, 0, 0), 0,
//                                                     "DESTINATION");
//
//    m_leGraphe.resize(m_leGraphe.getNbSommets() + 2);
//
//    m_sommetOrigine = m_leGraphe.getNbSommets() - 2;
//    m_sommetDestination = m_leGraphe.getNbSommets() - 1;
//
//    // Ajouts des sommets dans le graphe
//    m_arretDuSommet.push_back(pointOrigine);
//    m_arretDuSommet.push_back(pointDestination);
//
//    m_sommetDeArret.insert({pointOrigine, m_sommetOrigine});
//    m_sommetDeArret.insert({pointDestination, m_sommetDestination});
//
//
//    const map<unsigned int, Station> &m_stations = p_gtfs.getStations();
//
//    for (auto station = m_stations.begin(); station != m_stations.end(); ++station) {
//        const Coordonnees &coordStation = station->second.getCoords();
//
//        double distanceMarcheOrigine = p_pointOrigine - coordStation;
//        double distanceMarcheDestination = p_pointDestination - coordStation;
//
//        if (distanceMarcheOrigine < distanceMaxMarche) {
//            const multimap<Heure, Arret::Ptr> &arretsStation = station->second.getArrets();
//
//            unsigned int secondesMarche = ((distanceMarcheOrigine / vitesseDeMarche) * 3600);
//            Heure tempsMarcheOrigine = heureDepart.add_secondes(secondesMarche);
//
//            multimap<Heure, Arret::Ptr>::const_iterator arretAccessible = arretsStation.lower_bound(tempsMarcheOrigine);
//
//            if (arretAccessible != arretsStation.end()) {
//                m_leGraphe.ajouterArc(m_sommetOrigine, m_sommetDeArret[(*arretAccessible).second],
//                                      (*arretAccessible).first - heureDepart);
//                ++m_nbArcsOrigineVersStations;
//            }
//        }
//        if (distanceMarcheDestination < distanceMaxMarche) {
//            const multimap<Heure, Arret::Ptr> &arretsStation = station->second.getArrets();
//
//            unsigned int tempsMarcheDestination = (distanceMarcheDestination / vitesseDeMarche) * 3600;
//
//            for (auto arret = arretsStation.begin(); arret != arretsStation.end(); ++arret) {
//                size_t sommetArret = m_sommetDeArret[(*arret).second];
//
//                m_leGraphe.ajouterArc(sommetArret, m_sommetDestination, tempsMarcheDestination);
//                m_sommetsVersDestination.push_back(sommetArret);
//                ++m_nbArcsStationsVersDestination;
//            }
//        }
//    }
//
//    if (m_nbArcsStationsVersDestination == 0 or m_nbArcsOrigineVersStations == 0){
//        throw logic_error("Aucun arrêt de bus n'est dans le rayon maximal de marche de la destination ou de l'origine");
//    }
//
//    // On indique que que les arcs origine/destination ont été ajoutés
//    m_origine_dest_ajoute = true;

}

//! \brief Remet ReseauGTFS dans l'était qu'il était avant l'exécution de ReseauGTFS::ajouterArcsOrigineDestination()
//! \param[in] p_gtfs: un objet DonneesGTFS
//! \throws logic_error si une incohérence est détecté lors de la modification du graphe
//! \post Enlève de ReaseauGTFS tous les arcs allant du point source vers un arrêt de station et ceux allant d'un arrêt de station vers la destination
//! \post assigne la variable m_origine_dest_ajoute à false (les points orignine et destination sont enlevés du graphe)
//! \post enlève les données de m_sommetsVersDestination
void ReseauGTFS::enleverArcsOrigineDestination()
{
//    if (m_origine_dest_ajoute == false) {
//        throw logic_error("Il n'y a pas d'arcs d'origine et de destination dans le graphe");
//    }
//
//    for (auto sommet = m_sommetsVersDestination.begin(); sommet != m_sommetsVersDestination.end(); ++sommet) {
//        m_leGraphe.enleverArc(*sommet, m_sommetDestination);
//    }
//    m_leGraphe.resize(m_leGraphe.getNbSommets() - 2);
//
//    // Allons chercher les pointeurs des sommets Origine et Destination
//    Arret::Ptr sommetOrigine = m_arretDuSommet[m_sommetOrigine];
//    Arret::Ptr sommetDestination = m_arretDuSommet[m_sommetDestination];
//
//    // Suppression m_sommetDeArret
//    m_sommetDeArret.erase(sommetOrigine);
//    m_sommetDeArret.erase(sommetDestination);
//
//    // Suppression des sommets dans m_arretDuSommet
//    m_arretDuSommet.pop_back();
//    m_arretDuSommet.pop_back();
//
//    // Mise à jour des paramètres du graphe
//    m_nbArcsOrigineVersStations = 0;
//    m_nbArcsStationsVersDestination = 0;
//    m_sommetsVersDestination.clear();
//    m_origine_dest_ajoute = false;
}


