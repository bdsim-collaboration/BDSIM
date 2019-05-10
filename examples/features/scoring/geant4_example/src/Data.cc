#include "Data.hh"
#include "B1RunAction.hh"
#include <fstream>


using namespace std;

Data::Data()
{
    fgInstance = this;
}

Data::~Data()
{}

//Singleton pattern
Data* Data::fgInstance = 0;
Data* Data::Instance()
{
    return fgInstance;
}

void Data::AfficheTable(const vector<vector<double> > Table)
{
    cout << "---------------------------------------" << endl;
    cout << "Affichage de la table xxx " << endl;
    unsigned i,j;
    for(i = 0; i< Table.size(); i++)
    {
        for (j = 0; j< Table[i].size(); j++)
        {
            cout  << Table[i][j] << "\t";
        }
        cout << endl;

    }
    cout << "---------------------------------------" << endl;
}

vector<vector<double> > Data::LoadTable(string filename)
{
    vector < vector <double> > Table;

    ifstream fichier(filename.c_str(), ios::in);  // on ouvre en lecture
    if(fichier)  // si l'ouverture a fonctionné
    {
        G4cout << filename << " Succesfuly Open"<< G4endl;

        vector <double> column;

        int currentRow = 0;
        string contenu;
        string value;

        while(getline(fichier, contenu))
        {
            size_t tabPosition = 0;

            while(tabPosition != std::string::npos)
            {
                tabPosition = contenu.find("\t");
                value = contenu.substr(0,tabPosition);
                column.push_back((strtod(value.c_str(),NULL)));
                contenu = contenu.substr(tabPosition+1);
            }

            Table.push_back(column);

            currentRow ++;
            column.clear();

        }
        fichier.close();
    }
    else
    {
        G4String msg = "Impossible d'ouvrir le fichier "+ filename;
        G4Exception("Data::LoadTable()","", FatalException,msg);
    }
    return Table;
}

