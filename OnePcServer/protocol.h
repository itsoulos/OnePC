#ifndef PROTOCOL_H
#define PROTOCOL_H
# include <QString>
#include <QDir>
const QString genesisserver="localhost";
const int     genesisport=39764;
const QString genesispath=QDir::homePath()+QDir::separator()+"Genesis"+QDir::separator();
const QString genesisdatabase=genesispath+"genesis.db3";
struct MachineRecord
{
    QString macAddress;
    QString operatingSystem;
    bool isActive;
};
/*
 *  Εδώ καθορίζουμε ποιοι κωδικοί σημαίνουν τι στα μηνύματα
 *  που ανταλλάσσονται μεταξύ του διακομιστή και των πελατών.
*/
# define CM_ASKSERVERFORCONNECT   "100"
# define CM_SERVERSENDCODE        "101"
# define CM_CLIENTDELETE          "102"
# define CM_SERVERSENDPROGRAM     "103"
# define CM_SERVERASKSTATUS       "104"
# define CM_CLIENTSENDSTATUS      "105"
# define CM_SERVERSENDTERMINATE   "106"
# define CM_SERVERASKPAUSE        "107"
# define CM_SERVERASKRESUME       "108"
# define CM_SERVERSENDMINIMUM     "109"
# define CM_CLIENTGETMEMORY       "110"
# define CM_CLIENTSETMEMORY       "111"
# define CM_SERVERSENDMEMORY      "112"
# define CM_CLIENTSENDGENOMES     "120"
# define CM_SERVERSENDGENOMES     "121"
# define CM_CLIENTASKGENOMES      "122"
# define CM_SERVERNOP             "200"

/*
 *  This is the protocol for genesis server
*/
#define PROTOCOL_REQUEST_CONNECT        100
#define PROTOCOL_REQUEST_START          102
#define PROTOCOL_REQUEST_DISCONNECT     103
#define PROTOCOL_REQUEST_CLIENTSTART    104
/*
 *  Εδώ καθορίζονται οι καταστάσεις στις οποίες βρίσκονται οι πελάτες.
*/

#define STATE_START         "991"
#define STATE_WAIT          "992"
#define STATE_RUN           "993"
#define STATE_PAUSE         "994"
#define STATE_FINISH        "995"
#endif // PROTOCOL_H
