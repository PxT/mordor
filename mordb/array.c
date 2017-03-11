/*
 *      Copyright (C) 1998 Brooke Paul
 * $Id: array.c,v 1.1 2001/03/10 02:09:23 develop Exp $
 *
 * $Log: array.c,v $
 * Revision 1.1  2001/03/10 02:09:23  develop
 * Initial revision
 *
 * Revision 1.2  2001/03/08 16:05:42  develop
 * *** empty log message ***
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "../include/morutil.h"

#include "mordb.h"

static char class_str[CLASS_COUNT][15] = 
	{ "None", "Assassin", "Barbarian", "Cleric",
	"Fighter", "Mage", "Paladin", "Ranger", "Thief", "Bard", "Monk",
	"Druid", "Alchemist", "Builder", "Caretaker","Dungeonmaster" };


static char race_str[RACE_COUNT][15] = 
	{ "Unknown", "Dwarf", "Elf", "Half-elf", "Halfling",
	"Human", "Orc", "Half-giant", "Gnome", "Troll", "Half-orc", 
	"Ogre","Dark-elf", "Goblin" };

static char race_adj[RACE_COUNT][15] = 
	{ "Unknown", "Dwarven", "Elven", "Half-elven",
	"Halfling", "Mannish", "Orcish", "Half-giant", "Gnomish", "Trollkin",
	"Half-orc", "Ogre", "Dark-elf", "Goblin" };


/* static char lev_title_guys[][8][20] = {
	{ "", "", "", "", "", "", "", "" },
	{ "Minor Assassin", "Rutterkin", "Killer", "Cutthroat",
	  "Murderer", "Executioner", "Expert Assassin", "Master Assassin" },
	{ "Grunt", "Tribesman", "Savage", "Brutalizer", 
	  "Dragon-slayer", "Barbarian Chief", "Destroyer", "Master Barbarian" },
	{ "Acolyte", "Healer", "Adept", "Priest",
	  "Cardinal", "Lama", "Patriarch", "High Priest" },
	{ "Veteran", "Warrior", "Punisher", "Pugilist",
	  "Myrmidon", "Hero", "Champion", "Lord" },
	{ "Apprentice", "Prestidigitator", "Necromancer", "Occultate",
	  "Thaumaturgist", "Enchanter", "Sorcerer", "Wizard" },
	{ "Gallant", "Protector", "Warder", "Guardian",
	  "Ordained Paladin", "Chevalier", "Justiciar", "Holy Warrior" },
	{ "Searcher", "Scout", "Tracker", "Underhillsman", 
	  "Royal Guide", "Pathfinder", "Ranger Knight", "Ranger Lord" },
	{ "Rogue", "Footpad", "Dervish", "Burglar",
	  "Sharper", "Magsman", "High Thief", "Master Thief" },
	{ "Jongleur", "Lyrist", "Sonneteer", "Skald",
	  "Minstrel", "Muse", "Bard", "Master Bard" },
	{ "Novice", "Initiate", "Brother", "Disciple", "Immaculate",  
	  "Master", "Superior Master", "Grand Master" },
	{ "Aspiriant", "Ovate", "Initiate", "Master Initiate",
	  "Druid", "Master Druid", "Archdruid", "Great Druid" },
	{ "Scholar", "Sage", "Artificer", "Runemaster",
	  "Alchemist", "Mechanician", "Archeus", "Master Archeus" },
	{ "Apprentice", "Carpenter", "Bricklayer", "Mason",
	  "Steelworker", "Craftsman", "Builder", "Master Builder" },
	{ "Builder", "Creator", "Slave", "Daemon",
	  "Addict", "Hero", "Dungeonmaster", "Caretaker" },
	{ "Builder", "Creator", "Programmer", "Dungeon Lord", 
	  "Immortal", "Divine Entity", "Addict", "Dungeonmaster"  }
}; 

static char lev_title_dolls[][8][20] = {
        { "", "", "", "", "", "", "", "" },
        { "Minor Assassin", "Rutterkin", "Killer", "Cutthroat",
          "Murderess", "Executioner", "Expert Assassin", "Master Assassin" },
        { "Grunt", "Tribeswoman", "Savage", "Brutalizer",
          "Dragon-slayer", "Barbarian Queen", "Destroyer", "Master Barbarian" },
        { "Nun", "Healer", "Adept", "Priestess",
          "Cardinal", "Lama", "Matriarch", "High Priestess" },
        { "Veteran", "Warrior", "Punisher", "Pugilist",
          "Myrmidon", "Hero", "Champion", "Lady" },
        { "Apprentice", "Prestidigitator", "Necromancer", "Occultate",
          "Thaumaturgist", "Enchantress", "Sorceress", "Wizard" },
        { "Gallant", "Protector", "Warder", "Guardian",
          "Ordained Paladin", "Chevalier", "Justiciar", "Holy Warrior" },
        { "Searcher", "Scout", "Tracker", "Underhillswoman",
          "Royal Guide", "Pathfinder", "Ranger Knight", "Ranger Lady" },
        { "Rogue", "Footpad", "Dervish", "Burglar",
          "Sharper", "Magswoman", "High Thief", "Master Thief" },
        { "Poet", "Lyrist", "Sonneteer", "Skald",
          "Minstrel", "Muse", "Bard", "Master Bard" },
        { "Novice", "Initiate", "Sister", "Disciple", "Immaculate",
          "Master", "Superior Master", "Grand Master" },
        { "Aspiriant", "Ovate", "Initiate", "Master Initiate",
          "Druid", "Master Druid", "Archdruid", "Great Druid" },
        { "Scholar", "Sage", "Artificer", "Runemaster",
          "Alchemist", "Mechanician", "Archeus", "Master Archeus" },
        { "Apprentice", "Carpenter", "Bricklayer", "Mason",
          "Steelworker", "Craftsman", "Builder", "Master Builder" },
        { "Builder", "Creator", "Slave", "Daemon",
          "Addict", "Hero", "Dungeonmaster", "Caretaker" },
        { "Builder", "Creator", "Programmer", "Dungeon Lord",
          "Immortal", "Divine Entity", "Addict", "Dungeonmaster"  }
};

*/

static char lev_title_guys[][8][21] = {
        { "", "", "", "", "", "", "", "" },
        { "Prowler", "Brawler", "Cutthroat", "Killer",
          "Butcher", "Murderer", "Executioner", "Dark Lord" },
        { "Tribesman", "Berserker", "Brutalizer", "Pillager",
          "Destroyer", "Chieftain", "Warlord", "Great Khan" },
        { "Acolyte", "Novice", "Curate", "Priest", "Bishop", "Patriarch", 
	  "High Priest", "Pious One" },
        { "Warrior", "Veteran", "Swashbuckler",
          "Knight-Captain", "Hero", "Superhero", "Champion", "Lord of the Realm" },
        { "Apprentice", "Prestidigitator", "Warlock", "Occultate",
          "Thaumaturge", "Conjurer", "Sorcerer", "Wizard" },
        { "Gallant", "Protector", "Warder", "Guardian",
          "Holy Warrior", "Crusader", "Justiciar", "Defender" },
        { "Guide", "Yeoman", "Hunter", "Scout",
          "Mountaineer", "Pathfinder", "Adventurer", "Royal Guide" },
        { "Footpad", "Mugger", "Thug", "Brigand",
          "Rogue", "Burglar", "Sharper", "Shadow Lord" },
      	{ "Musician", "Singer", "Rhymer", "Skald", 
          "Minstrel", "Troubadour", "Poet Laureate", "Royal Bard" },
        { "Pupil", "Brother", "Disciple", "Sage", "Lama", "Master", "Grand Master", "Enlightened One" },
        { "Aspiriate", "Initiate", "Naturalist", "Shaman",
          "Druid Warrior", "Elementalist", "Elemental Master", "Ancient One" },
        { "Symbolist", "Transmuter", "Artificer", "Runemaster",
          "Mechanician", "Enchanter", "Savant", "Archeus" },
        { "Apprentice Builder", "Apprentice Builder", "Builder", "Builder",
          "Creator", "Creator", "Master Builder", "Master Builder" },
        { "Apprentice Caretaker", "Novice Caretaker", "Caretaker", "Senior Caretaker",
          "Leviathan Caretaker", "Demigod", "Denizen", "Inquisitor" },
        { "Dungeon Lord", "Dungeon Master", "Dark Lord", "Death Master",
	  "Immortal", "Divine Entity", "Supreme Being", "Grand Inquisitor" }
};

static char lev_title_dolls[][8][21] = {
        { "", "", "", "", "", "", "", "" },
        { "Prowler", "Lurker", "Stalker", "Viper",
          "Blade", "Murderess", "Femme Fatale", "Dark Mistress" },
        { "Tribeswoman", "Berserker Maiden", "Amazon", "Tigress",
          "Destroyer", "Chieftess", "Warrior Princess", "Amazon Queen" },
        { "Acolyte", "Healer", "Postulant", "Priestess", "Prioress", 
	  "Matriarch", "High Priestess", "Oracle" },
        { "Warrior Maiden", "Mercenary", "Shield Maiden",
          "Myrmidon", "Heroine", "Superheroine", "Championess", "Lady of the Realm" },
        { "Apprentice", "Medium", "Witch", "Mystic",
          "Cabbalist", "Conjuress", "Sorceress", "Coven Mistress" },
        { "Gallant", "Protectress", "Sentinel", "Guardian",
          "Holy Avenger", "Crusader", "Seneschal", "Royal Sentinel" },
        { "Guide", "Wanderess", "Huntress", "Tracker", 
          "Explorer", "Trailblazer", "Adventuress", "Royal Forester" },
        { "Footpad", "Pickpocket", "Cutpurse", "Purloiner",
          "Swindler", "Cat Burglar", "Magswoman", "Shadow Mistress" },
      	{ "Musician", "Lyricist", "Poet", "Sonneteer",
          "Minstrel", "Muse", "Poet Laureate", "Royal Muse" },
        { "Pupil", "Sister", "Disciple", "Immaculate", "Mother Superior",
          "Mistress", "Grand Mistress", "Superior One" },
        { "Aspiriate", "Sister Initiate", "Forester", "Ovate",
          "Forest Warden", "Elementalist", "Elemental Mistress", "Druid Queen" },
	{ "Symbolist", "Scholar", "Artisan", "Runemistress",
          "Mechanician", "Enchantress", "Savant", "Adeptus" },
        { "Apprentice Builder", "Apprentice Builder", "Builder", "Builder",
          "Creator", "Creator", "Mistress Builder", "Mistress Builder" },
        { "Apprentice Caretaker", "Apprentice Caretaker", "Caretaker", "Senior Caretaker",
           "Leviathan", "Demigoddess", "Denizen", "Inquisitor" },
        { "Dungeon Mistress", "Dungeon Queen", "Dark Queen", "Death Mistress",
          "Princess", "Divine Entity", "Supreme Being", "Grand Inquisitor" }
};

static int loaded_hash_yet = FALSE;
int HASHROOMS = 0;


/******************************************************************/
/* get_class_string()						  */
/******************************************************************/
char *get_class_string( int nIndex )
{
	/* do bounds checking */
	nIndex =   MAX( 0, MIN(nIndex, CLASS_COUNT - 1 ) );

	return( class_str[nIndex] );
}


/******************************************************************/
/* get_reace_string()						  */
/******************************************************************/
char *get_race_string( int nIndex )
{
	/* do bounds checking */
	nIndex =   MAX( 0, MIN(nIndex, RACE_COUNT - 1 ) );

	return( race_str[nIndex] );
}

/******************************************************************/
/* get_race_adjective()						  */
/******************************************************************/
char *get_race_adjective( int nIndex )
{
	/* do bounds checking */
	nIndex =   MAX( 0, MIN(nIndex, RACE_COUNT - 1 ) );

	return( race_adj[nIndex] );
}


/**********************************************************************/
/*				title_ply			      */
/**********************************************************************/

/* This function returns a string with the player's character title in */
/* it.  The title is determined by looking at the player's class and   */
/* level.							       */

char *title_ply(creature *ply_ptr )
{
	int	titlnum;

	ASSERTLOG( ply_ptr );

	titlnum = (ply_ptr->level-1)/3;
	if(titlnum > 7)
		titlnum = 7;
	if(F_ISSET(ply_ptr, PMALES)) {
		return(lev_title_guys[(int)ply_ptr->class][titlnum]);
	} 
	else
	{
		return(lev_title_dolls[(int)ply_ptr->class][titlnum]);
	}
}




/********************************************************************/
/* get_hash_rooms()			                            */
/********************************************************************/
int get_hash_rooms()
{
	char	szBuffer[80];
	char	filename[256];

	if ( !loaded_hash_yet )
	{
		if (get_config_file( filename ) )
		{
			GetIniString( "Game Settings", "HashRooms", "0", szBuffer, 80, filename);
			HASHROOMS = convert_to_bool( szBuffer );
		}
	}


	return( HASHROOMS );
}
