#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <limits>

using namespace std;

class Korta {
private:
	string rangas;
	string tipas;

public:
	Korta(string r, string t) : rangas(r), tipas(t) {}

	string getRangas() const {
		return rangas; 
	}

	string getTipas() const {
		return tipas;
	}

	string getPavadinimas() const {
		return rangas + " " + tipas;
	}
};
class IKalade {
public:
	virtual ~IKalade() = default;
	virtual void sumaisyti() = 0;
	virtual bool tuscia() const = 0;
	virtual Korta paimtiKorta() = 0;
	virtual void reset() = 0;
};
class Kalade : public IKalade {
private:
	vector<Korta> kortos;

	void pripildytiKalade() {
		kortos.clear();
		vector<string> tipai = { "Piku", "Bugnu", "Kryziu", "Sirdziu" };
		vector<string> rangai = { "2","3","4","5","6","7","8","9","10","J","Q","K","A" };

		for (const auto& t : tipai) {
			for (const auto& r : rangai) {
				kortos.emplace_back(r, t);
			}
		}
	}

public:
	Kalade() { pripildytiKalade(); }

	void reset() override {
		pripildytiKalade();
	}

	void sumaisyti() override {
		random_device rd;
		mt19937 gen(rd());
		shuffle(kortos.begin(), kortos.end(), gen);
	}

	bool tuscia() const override {
		return kortos.empty();
	}

	Korta paimtiKorta() override {
		if (tuscia()) throw runtime_error("Kalade tuscia");
		Korta virsus = kortos.back();
		kortos.pop_back();
		return virsus;
	}
};
class Ranka {
private:
	vector<Korta> kortos;

public:
	void pridetiKorta(const Korta& k) {
		kortos.push_back(k);
	}

	int gautiSuma() const {
		int suma = 0;
		int tuzai = 0;

		for (const auto& k : kortos) {
			string r = k.getRangas();

			if (r == "J" || r == "Q" || r == "K") suma += 10;
			else if (r == "A") { suma += 11; tuzai++; }
			else suma += stoi(r);
		}

		while (suma > 21 && tuzai > 0) {
			suma -= 10;
			tuzai--;
		}
		return suma;
	}

	bool yraBust() const {
		return gautiSuma() > 21;
	}

	bool arTuriBlackjack() const {
		return (gautiSuma() == 21 && kortos.size() == 2);
	}

	string getPavadinimas() const {
		string tekstas;
		for (size_t i = 0; i < kortos.size(); ++i) {
			tekstas += kortos[i].getPavadinimas();
			if (i < kortos.size() - 1) tekstas += ", ";
		}
		return tekstas;
	}

	size_t getKiekis() const {
		return kortos.size();
	}

	void isvalyti() {
		kortos.clear();
	}

	string getPirmaKorta() const {
		if (kortos.empty()) return "";
		return kortos[0].getPavadinimas();
	}
};
class Pinigine {
private:
	int balansas;
	int statymas;

public:
	Pinigine(int pradinisBalansas) : balansas(pradinisBalansas), statymas(0) {}

	int getBalansas() const {
		return balansas;
	}
	int getStatymas() const {
		return statymas;
	}

	bool nustatytStatyma(int suma) {
		if (suma > 0 && suma <= balansas) {
			statymas = suma;
			return true;
		}
		statymas = 0;
		return false;
	}

	void laimejo() {
		balansas += statymas;
		statymas = 0;
	}

	void pralaimejo() {
		balansas -= statymas;
		statymas = 0;
	}

	void lygiosios() {
		statymas = 0;
	}
};
class Dalyvis {
protected:
	Ranka ranka;

public:
	virtual ~Dalyvis() = default;

	void hit(IKalade& kalade) {
		ranka.pridetiKorta(kalade.paimtiKorta());
	}
	int gautiSuma() const {
		return ranka.gautiSuma();
	}
	bool yraBust() const {
		return ranka.yraBust();
	}
	bool arTuriBlackjack() const {
		return ranka.arTuriBlackjack();
	}
	string getRankosTekstas() const {
		return ranka.getPavadinimas();
	}
	string getPirmaKortaTekstas() const {
		return ranka.getPirmaKorta();
	}
	void resetRanka() {
		ranka.isvalyti();
	}
	virtual string getVardas() const = 0;
};
class IZaidejas : public Dalyvis {
public:
	virtual ~IZaidejas() = default;

	virtual bool nustatytiStatyma(int suma) = 0;
	virtual int getBalansas() const = 0;
	virtual int getStatymas() const = 0;
	virtual void laimejo() = 0;
	virtual void pralaimejo() = 0;
	virtual void lygiosios() = 0;
	virtual void resetRanka() = 0;
};
class Zaidejas : public IZaidejas {
private:
	string vardas;
	Pinigine pinigine;

public:
	Zaidejas(const string& v, int pradinisBalansas) : vardas(v), pinigine(pradinisBalansas) {}
	string getVardas() const override {
		return vardas;
	}

	bool nustatytiStatyma(int suma) override {
		return pinigine.nustatytStatyma(suma);
	}
	int getBalansas() const override {
		return pinigine.getBalansas();
	}
	int getStatymas() const override {
		return pinigine.getStatymas();
	}
	void laimejo() override {
		pinigine.laimejo();
	}
	void pralaimejo() override {
		pinigine.pralaimejo();
	}
	void lygiosios() override {
		pinigine.lygiosios();
	}
	void resetRanka() override {
		Dalyvis::resetRanka();
	}
};
class Dyleris : public Dalyvis {
public:
	string getVardas() const override {
		return "Dyleris";
	}

	void suzaistiEiga(IKalade& kalade) {
		while (gautiSuma() < 17 && !yraBust()) {
			hit(kalade);
		}
	}
};
class IGameIO {
public:
	virtual ~IGameIO() = default;
	virtual void spausdinti(const string& text) = 0;
	virtual char skaitytiChar() = 0;
	virtual int skaitytiInt() = 0;
};
class GameIO : public IGameIO {
public:
	void spausdinti(const string& text) override {
		cout << text;
	}
	char skaitytiChar() override {
		char c;
		cin >> c;
		return c;
	}
	int skaitytiInt() override {
		int x;
		cin >> x;
		return x;
	}
};
class Game {
private:
	IKalade& kalade;
	IGameIO& io;
	IZaidejas& zaidejas;

	void skirtukas() {
		io.spausdinti("=========================================================\n");
	}
	bool arZaisiRaunda() {
		while (true) {
			skirtukas();
			io.spausdinti("Ar nori suzaisti raunda? (y/n): ");
			char c = io.skaitytiChar();

			if (c == 'y' || c == 'Y') return true;
			if (c == 'n' || c == 'N') return false;

			io.spausdinti("Neteisingas pasirinkimas. Ivesk tik y arba n.\n");
		}
	}
	void paruosktiNaujaRaunda(Dyleris& dyleris) {
		zaidejas.resetRanka();
		dyleris.resetRanka();

		kalade.reset();
		kalade.sumaisyti();

		skirtukas();
		io.spausdinti("Tavo balansas: " + to_string(zaidejas.getBalansas()) + "\n");
	}
	void gautiStatymaIsZaidejo() {
		while (true) {
			io.spausdinti("Kiek statysi?: ");
			int suma = io.skaitytiInt();

			if (zaidejas.nustatytiStatyma(suma)) break;
			io.spausdinti("Neteisingas statymas. Bandyk dar karta.\n");
		}
	}
	void isdalintiPradinesKortas(Dyleris& dyleris) {
		zaidejas.hit(kalade);
		dyleris.hit(kalade);
		zaidejas.hit(kalade);
		dyleris.hit(kalade);
	}
	void spausdintiPradineSituacija(const Dyleris& dyleris) {
		skirtukas();
		io.spausdinti("PRADINE SITUACIJA\n");

		io.spausdinti(
			zaidejas.getVardas() + " ranka: " +
			zaidejas.getRankosTekstas() +
			" (suma: " + to_string(zaidejas.gautiSuma()) + ")\n"
		);

		io.spausdinti(
			dyleris.getVardas() + " ranka: " +
			dyleris.getPirmaKortaTekstas() +
			", [uzversta korta] (suma: ?)\n"
		);
	}
	void apdorotiBlackjack(const Dyleris& dyleris) {
		skirtukas();
		io.spausdinti("BLACKJACK!\n");

		bool zBJ = zaidejas.arTuriBlackjack();
		bool dBJ = dyleris.arTuriBlackjack();

		if (zBJ && dBJ) {
			io.spausdinti("Abu turi blackjack -> LYGIOSIOS.\n");
			zaidejas.lygiosios();
		}
		else if (zBJ) {
			io.spausdinti("Tu turi blackjack -> LAIMEJAI.\n");
			zaidejas.laimejo();
		}
		else {
			io.spausdinti("Dyleris turi blackjack -> PRALAIMEJAI.\n");
			zaidejas.pralaimejo();
		}

		io.spausdinti("Naujas balansas: " + to_string(zaidejas.getBalansas()) + "\n");
	}
	bool zaidejoEiga() {
		while (true) {
			skirtukas();
			io.spausdinti("Pasirink (h)it arba (s)tand: ");
			char pasirinkimas = io.skaitytiChar();

			if (pasirinkimas == 'h' || pasirinkimas == 'H') {
				zaidejas.hit(kalade);

				skirtukas();
				io.spausdinti(
					zaidejas.getVardas() + " ranka: " +
					zaidejas.getRankosTekstas() +
					" (suma: " + to_string(zaidejas.gautiSuma()) + ")\n"
				);

				if (zaidejas.yraBust()) {
					io.spausdinti("Prakisai (BUST)\n");
					zaidejas.pralaimejo();
					io.spausdinti("Naujas balansas: " + to_string(zaidejas.getBalansas()) + "\n");
					return false;
				}

				if (zaidejas.gautiSuma() == 21) {
					io.spausdinti("Turi 21, nebereikia traukti.\n");
					return true;
				}
			}
			else if (pasirinkimas == 's' || pasirinkimas == 'S') {
				return true;
			}
			else {
				io.spausdinti("Neteisingas pasirinkimas.\n");
			}
		}
	}
	void dylerioEiga(Dyleris& dyleris) {
		skirtukas();
		io.spausdinti("DYLERIO EJIMAS\n");

		io.spausdinti(
			dyleris.getVardas() + " ranka: " +
			dyleris.getRankosTekstas() +
			" (suma: " + to_string(dyleris.gautiSuma()) + ")\n"
		);

		dyleris.suzaistiEiga(kalade);

		io.spausdinti(
			dyleris.getVardas() + " ranka: " +
			dyleris.getRankosTekstas() +
			" (suma: " + to_string(dyleris.gautiSuma()) + ")\n"
		);
	}
	void apdorotiDylerioBust() {
		skirtukas();
		io.spausdinti("Dylerio BUST! Laimejai.\n");
		zaidejas.laimejo();
		io.spausdinti("Naujas balansas: " + to_string(zaidejas.getBalansas()) + "\n");
	}
	void apdorotiRezultata(const Dyleris& dyleris) {
		int sumaZ = zaidejas.gautiSuma();
		int sumaD = dyleris.gautiSuma();

		skirtukas();
		io.spausdinti("GALUTINES SUMOS\n");
		io.spausdinti(zaidejas.getVardas() + ": " + to_string(sumaZ) + "\n");
		io.spausdinti(dyleris.getVardas() + ": " + to_string(sumaD) + "\n");

		if (sumaZ > sumaD) {
			io.spausdinti("Laimejo " + zaidejas.getVardas() + "\n");
			zaidejas.laimejo();
		}
		else if (sumaD > sumaZ) {
			io.spausdinti("Laimejo " + dyleris.getVardas() + "\n");
			zaidejas.pralaimejo();
		}
		else {
			io.spausdinti("Lygiosios\n");
			zaidejas.lygiosios();
		}

		io.spausdinti("Naujas balansas: " + to_string(zaidejas.getBalansas()) + "\n");
	}
	void spausdintiZaIdimoPabaiga(int pradziosBalansas) {
		skirtukas();
		io.spausdinti("ZAIDIMAS BAIGTAS\n");
		io.spausdinti("Pradinis balansas: " + to_string(pradziosBalansas) + "\n");
		io.spausdinti("Galutinis balansas: " + to_string(zaidejas.getBalansas()) + "\n");
		io.spausdinti("Pelnas: " + to_string(zaidejas.getBalansas() - pradziosBalansas) + "\n");
	}

public:
	Game(IKalade& k, IGameIO& io_, IZaidejas& z) : kalade(k), io(io_), zaidejas(z) {}

	void run() {
		int pradziosBalansas = zaidejas.getBalansas();
		while (zaidejas.getBalansas() > 0) {
			if (!arZaisiRaunda()) break;

			Dyleris dyleris;
			paruosktiNaujaRaunda(dyleris);
			gautiStatymaIsZaidejo();

			isdalintiPradinesKortas(dyleris);
			spausdintiPradineSituacija(dyleris);

			if (zaidejas.arTuriBlackjack() || dyleris.arTuriBlackjack()) {
				apdorotiBlackjack(dyleris);
				continue;
			}

			if (!zaidejoEiga()) {
				continue;
			}

			dylerioEiga(dyleris);

			if (dyleris.yraBust()) {
				apdorotiDylerioBust();
				continue;
			}

			apdorotiRezultata(dyleris);
		}
		spausdintiZaIdimoPabaiga(pradziosBalansas);
	}
};

int main() {
	GameIO io;

	io.spausdinti("Zaidejo vardas: ");
	string vardas;
	cin >> vardas;

	io.spausdinti("Ivesk savo balansa: ");
	int pradinisBalansas;
	cin >> pradinisBalansas;

	Kalade kalade;
	Zaidejas z(vardas, pradinisBalansas);
	Game game(kalade, io, z);
	game.run();

	return 0;
}