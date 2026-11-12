#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

// Programme compatible avec le standard C++98 de Dev-C++ 5.11.

class Matiere {
private:
    string code;
    string nom;

public:
    Matiere(const string& c, const string& n) : code(c), nom(n) {}

    string getCode() const { return code; }
    string getNom() const { return nom; }
};

class Note {
private:
    string codeMatiere;
    double valeur;

public:
    Note(const string& code, double v) : codeMatiere(code), valeur(v) {
        if (v < 0.0 || v > 20.0) {
            throw invalid_argument("Une note doit etre comprise entre 0 et 20.");
        }
    }

    string getCodeMatiere() const { return codeMatiere; }
    double getValeur() const { return valeur; }
};

class UE {
private:
    string code;
    string nom;
    int credits;
    vector<Matiere> matieres;

public:
    UE(const string& c, const string& n, int cr) : code(c), nom(n), credits(cr) {
        if (cr <= 0) {
            throw invalid_argument("Une UE doit avoir au moins un credit.");
        }
    }

    void ajouterMatiere(const Matiere& matiere) {
        unsigned int i;
        for (i = 0; i < matieres.size(); i++) {
            if (matieres[i].getCode() == matiere.getCode()) {
                throw invalid_argument("Code matiere deja present dans cette UE.");
            }
        }
        matieres.push_back(matiere);
    }

    string getCode() const { return code; }
    string getNom() const { return nom; }
    int getCredits() const { return credits; }
    const vector<Matiere>& getMatieres() const { return matieres; }
};

class Filiere {
private:
    string code;
    string nom;
    vector<UE> ues;

public:
    Filiere(const string& c, const string& n) : code(c), nom(n) {}

    void ajouterUE(const UE& ue) {
        unsigned int i;
        for (i = 0; i < ues.size(); i++) {
            if (ues[i].getCode() == ue.getCode()) {
                throw invalid_argument("Code UE deja present dans cette filiere.");
            }
        }
        ues.push_back(ue);
    }

    string getCode() const { return code; }
    string getNom() const { return nom; }
    const vector<UE>& getUEs() const { return ues; }
};

class Etudiant {
private:
    string matricule;
    string nom;
    string niveau;
    const Filiere* filiere;
    vector<Note> notes;

    bool matiereExiste(const string& codeMatiere) const {
        unsigned int i, j;
        const vector<UE>& ues = filiere->getUEs();
        for (i = 0; i < ues.size(); i++) {
            const vector<Matiere>& matieres = ues[i].getMatieres();
            for (j = 0; j < matieres.size(); j++) {
                if (matieres[j].getCode() == codeMatiere) return true;
            }
        }
        return false;
    }

    int indiceNote(const string& codeMatiere) const {
        unsigned int i;
        for (i = 0; i < notes.size(); i++) {
            if (notes[i].getCodeMatiere() == codeMatiere) return (int)i;
        }
        return -1;
    }

public:
    Etudiant(const string& mat, const string& n, const string& niv,
             const Filiere& f)
        : matricule(mat), nom(n), niveau(niv), filiere(&f) {
        if (niv != "L1" && niv != "L2" && niv != "L3") {
            throw invalid_argument("Le niveau doit etre L1, L2 ou L3.");
        }
    }

    void ajouterNote(const Note& note) {
        if (!matiereExiste(note.getCodeMatiere())) {
            throw invalid_argument("Cette matiere n'appartient pas a la filiere.");
        }
        if (indiceNote(note.getCodeMatiere()) != -1) {
            throw invalid_argument("Une note existe deja pour cette matiere.");
        }
        notes.push_back(note);
    }

    double moyenneGenerale() const {
        const vector<UE>& ues = filiere->getUEs();
        double sommePonderee = 0.0;
        int totalCredits = 0;
        unsigned int i, j;

        for (i = 0; i < ues.size(); i++) {
            const vector<Matiere>& matieres = ues[i].getMatieres();
            double sommeUE = 0.0;
            int nombreNotes = 0;
            for (j = 0; j < matieres.size(); j++) {
                int index = indiceNote(matieres[j].getCode());
                if (index != -1) {
                    sommeUE += notes[index].getValeur();
                    nombreNotes++;
                }
            }
            if (nombreNotes > 0) {
                sommePonderee += (sommeUE / nombreNotes) * ues[i].getCredits();
                totalCredits += ues[i].getCredits();
            }
        }
        if (totalCredits == 0) {
            throw logic_error("Aucune note disponible pour calculer la moyenne.");
        }
        return sommePonderee / totalCredits;
    }

    bool valideUE(const string& codeUE) const {
        const vector<UE>& ues = filiere->getUEs();
        unsigned int i, j;
        for (i = 0; i < ues.size(); i++) {
            if (ues[i].getCode() == codeUE) {
                const vector<Matiere>& matieres = ues[i].getMatieres();
                if (matieres.size() == 0) return false;
                double somme = 0.0;
                for (j = 0; j < matieres.size(); j++) {
                    int index = indiceNote(matieres[j].getCode());
                    if (index == -1) return false;
                    somme += notes[index].getValeur();
                }
                return (somme / matieres.size()) >= 10.0;
            }
        }
        throw invalid_argument("UE inconnue dans la filiere de l'etudiant.");
    }

    void afficherReleve() const {
        const vector<UE>& ues = filiere->getUEs();
        unsigned int i, j;
        cout << "Releve de notes - " << nom << " (" << matricule << ")" << endl;
        cout << "Filiere : " << filiere->getNom() << " | Niveau : " << niveau << endl;
        cout << fixed << setprecision(2);

        for (i = 0; i < ues.size(); i++) {
            const vector<Matiere>& matieres = ues[i].getMatieres();
            double somme = 0.0;
            int nombreNotes = 0;
            cout << endl << ues[i].getCode() << " - " << ues[i].getNom()
                 << " (" << ues[i].getCredits() << " credits)" << endl;

            for (j = 0; j < matieres.size(); j++) {
                int index = indiceNote(matieres[j].getCode());
                cout << "  " << matieres[j].getNom() << " : ";
                if (index != -1) {
                    cout << notes[index].getValeur() << "/20" << endl;
                    somme += notes[index].getValeur();
                    nombreNotes++;
                } else {
                    cout << "non notee" << endl;
                }
            }

            if (nombreNotes == (int)matieres.size() && nombreNotes > 0) {
                double moyenneUE = somme / nombreNotes;
                cout << "  Moyenne UE : " << moyenneUE << "/20 - "
                     << (moyenneUE >= 10.0 ? "Validee" : "Non validee") << endl;
            } else {
                cout << "  Moyenne UE : indisponible (notes manquantes)" << endl;
            }
        }

        if (notes.size() > 0) {
            cout << endl << "Moyenne generale ponderee par les credits : "
                 << moyenneGenerale() << "/20" << endl;
        } else {
            cout << endl << "Moyenne generale : indisponible (aucune note)" << endl;
        }
    }
};

int main() {
    try {
        Filiere info("INFO", "Informatique");

        UE ueProgrammation("UE101", "Programmation", 6);
        ueProgrammation.ajouterMatiere(Matiere("MAT101", "Algorithmique"));
        ueProgrammation.ajouterMatiere(Matiere("MAT102", "C++"));
        info.ajouterUE(ueProgrammation);

        UE ueMathematiques("UE102", "Mathematiques", 4);
        ueMathematiques.ajouterMatiere(Matiere("MAT201", "Analyse"));
        info.ajouterUE(ueMathematiques);

        Etudiant etudiant("E001", "Amina Diallo", "L1", info);
        etudiant.ajouterNote(Note("MAT101", 14.0));
        etudiant.ajouterNote(Note("MAT102", 12.0));
        etudiant.ajouterNote(Note("MAT201", 9.0));

        etudiant.afficherReleve();
        cout << endl << "Validation de UE101 : "
             << (etudiant.valideUE("UE101") ? "oui" : "non") << endl;
    } catch (const exception& erreur) {
        cerr << "Erreur : " << erreur.what() << endl;
        return 1;
    }
    return 0;
}

