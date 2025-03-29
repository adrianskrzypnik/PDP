with Ada.Text_IO;
with Ada.Real_Time;
with Ada.Synchronous_Task_Control;
use Ada.Text_IO;
use Ada.Real_Time;
use Ada.Synchronous_Task_Control;

procedure Palacze_Fajek is

   k : constant Integer := 3; -- liczba palaczy
   l : constant Integer := 2; -- liczba ubijaczy
   m : constant Integer := 2; -- liczba pudelek zapalek

   protected Ubijacz is
      entry Pobierz;
      procedure Zwroc;
   private
      Dostepne : Integer := l;
   end Ubijacz;

   protected body Ubijacz is
      entry Pobierz when Dostepne > 0 is
      begin
         Dostepne := Dostepne - 1;
      end Pobierz;

      procedure Zwroc is
      begin
         Dostepne := Dostepne + 1;
      end Zwroc;
   end Ubijacz;

   protected Zapalki is
      entry Pobierz;
      procedure Zwroc;
   private
      Dostepne : Integer := m;
   end Zapalki;

   protected body Zapalki is
      entry Pobierz when Dostepne > 0 is
      begin
         Dostepne := Dostepne - 1;
      end Pobierz;

      procedure Zwroc is
      begin
         Dostepne := Dostepne + 1;
      end Zwroc;
   end Zapalki;

   protected Ekran is
      entry Wypisz(Msg : in String);
   end Ekran;

   protected body Ekran is
      entry Wypisz(Msg : in String) when True is
      begin
         Put_Line(Msg);
      end Wypisz;
   end Ekran;

   task type Palacz(ID : Integer);

   task body Palacz is
   begin
      loop
         Ekran.Wypisz("Palacz" & ID'Img & " czeka na ubijacz.");
         Ubijacz.Pobierz;
         Ekran.Wypisz("Palacz" & ID'Img & " ubija tyton.");
         delay 1.0;
         Ubijacz.Zwroc;
         Ekran.Wypisz("Palacz" & ID'Img & " zwrocil ubijacz.");

         Ekran.Wypisz("Palacz" & ID'Img & " czeka na pudelko zapalek.");
         Zapalki.Pobierz;
         Ekran.Wypisz("Palacz" & ID'Img & " zapala fajke.");
         delay 1.0;
         Zapalki.Zwroc;
         Ekran.Wypisz("Palacz" & ID'Img & " zwrocil pudelko zapalek.");

         Ekran.Wypisz("Palacz" & ID'Img & " pali fajke.");
         delay 2.0;
      end loop;
   end Palacz;

   type Palacz_Access is access Palacz;
   Palacze : array(1..k) of Palacz_Access;

begin
   for I in 1..k loop
      Palacze(I) := new Palacz(I);
   end loop;

   loop
      delay 10.0;
   end loop;
end Palacze_Fajek;
