#include "plugin.h"
#include "CTimer.h"
#include "extensions/ScriptCommands.h"

using namespace plugin;

class OpenDoorAnim
{
public:
	OpenDoorAnim()
	{
		class PedExtData
		{
		public:
			unsigned int animDoorLastTime;

			PedExtData(CPed *ped)
			{
				animDoorLastTime = CTimer::m_snTimeInMilliseconds;
			}
		};
		static PedExtendedData<PedExtData> pedExtData;

		Events::objectRenderEvent += [](CObject *obj)
		{
			if (obj->m_pObjectInfo->m_nSpecialColResponseCase == 6) // SWINGDOOR
			{
				obj->m_nPhysicalFlags.bCanBeCollidedWith = true; // record collisions

				for (int i = 0; i < 6; ++i)
				{
					if (obj->m_nNumEntitiesCollided > 0)
					{
						CEntity *entity = obj->m_apCollidedEntities[i];
						if (entity && entity->m_nType == ENTITY_TYPE_PED)
						{
							CPed *ped = reinterpret_cast<CPed*>(entity);
							PedExtData &xdata = pedExtData.Get(ped);
							if ((CTimer::m_snTimeInMilliseconds - xdata.animDoorLastTime) > 1700)
							{
								if (!ped->m_pIntelligence->GetTaskUseGun() && ped->m_fHealth > 0.0f)
								{
									Command<Commands::TASK_PLAY_ANIM_SECONDARY>(ped, "WALK_DOORPARTIAL", "PED", 4.0f, 0, 0, 0, 0, -1); // TODO improve me
									xdata.animDoorLastTime = CTimer::m_snTimeInMilliseconds;
								}
							}
						}
					}
				}
			}

		};

	}
} openDoorAnim;
