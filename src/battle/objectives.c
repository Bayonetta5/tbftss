/*
Copyright (C) 2015 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "objectives.h"

void completeConditions(void);
void failIncompleteObjectives(void);

void doObjectives(void)
{
	int objectiveFailed;
	Objective *o;
	
	battle.numObjectivesComplete = battle.numObjectivesTotal = 0;
	objectiveFailed = 0;
	
	for (o = battle.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (!o->isCondition)
		{
			battle.numObjectivesTotal++;
		}
		
		if (o->currentValue == o->targetValue)
		{
			switch (o->status)
			{
				case OS_COMPLETE:
					battle.numObjectivesComplete++;
					break;
					
				case OS_FAILED:
					if (!o->isOptional)
					{
						objectiveFailed = 1;
					}
					break;
			}
		}
	}
	
	if (battle.status == MS_IN_PROGRESS)
	{
		if (battle.numObjectivesTotal > 0 && battle.numObjectivesComplete == battle.numObjectivesTotal)
		{
			battle.status = MS_COMPLETE;
			battle.missionFinishedTimer = FPS;
			
			game.stats.missionsCompleted++;
			
			completeConditions();
			
			updateChallenges();
		}
		
		if (objectiveFailed)
		{
			battle.status = MS_FAILED;
			battle.missionFinishedTimer = FPS;
			
			failIncompleteObjectives();
		}
	}
}

void updateObjective(char *name, int type)
{
	Objective *o;
	
	for (o = battle.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (!o->isCondition && o->targetType == type && o->currentValue < o->targetValue && strcmp(o->targetName, name) == 0)
		{
			o->currentValue++;
			
			if (o->targetValue - o->currentValue <= 10)
			{
				addHudMessage(colors.cyan, "%s - %d / %d", o->description, o->currentValue, o->targetValue);
			}
			else if (o->currentValue % 10 == 0)
			{
				addHudMessage(colors.cyan, "%s - %d / %d", o->description, o->currentValue, o->targetValue);
			}
			
			if (o->currentValue == o->targetValue)
			{
				o->status = OS_COMPLETE;
				addHudMessage(colors.green, "%s - Objective Complete!", o->description);
			}
		}
	}
}

void updateCondition(char *name, int type)
{
	Objective *o;
	
	for (o = battle.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (o->isCondition && o->targetType == type && o->currentValue < o->targetValue && strcmp(o->targetName, name) == 0)
		{
			o->currentValue++;
			
			if (o->currentValue == o->targetValue)
			{
				o->status = OS_FAILED;
				addHudMessage(colors.red, "%s - Objective Failed!", o->description);
			}
		}
	}
}

void completeConditions(void)
{
	Objective *o;
	
	for (o = battle.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (o->isCondition)
		{
			o->status = OS_COMPLETE;
		}
	}
}


void failIncompleteObjectives(void)
{
	Objective *o;
	
	for (o = battle.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (o->status != OS_COMPLETE)
		{
			o->status = OS_FAILED;
		}
	}
}
