#include <stdio.h>
#include <iostream.h>
#include <String.h>

class Player {
public:
  String id;
  String player;
  String clan;
  String street;
  String suburb;
  String city;
  String country;
  String credit;
  String output;
  Player() {};
  void Dump(void) {
    cout << id << "\n";
    cout << player << "\n";
    cout << clan << "\n";
    cout << street << "\n";
    cout << suburb << "\n";
    cout << city << "\n";
    cout << country << "\n";
  };
};

int main(int argc, char *argv[])
{
  FILE *f;
  char *buf;
  f = fopen("DATA/LPLAYER.007", "r");
  buf = new char[512];
  fgets(buf, 512, f);
  String date = buf;
  while (fgets(buf, 512, f))
    {
      String array[10];
      String input = buf;
      String div = ",";
      split(input, array, 10, div);
      Player player;
      player.id = array[0];
      player.player = array[1];
      player.clan = array[2];
      player.street = array[3];
      player.suburb = array[4];
      player.city = array[5];
      player.country = array[6];
      player.credit = array[7];
      player.output = array[8];
      player.Dump();
    }
}
