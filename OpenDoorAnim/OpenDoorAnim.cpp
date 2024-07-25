#include "plugin.h"
#include "CTimer.h"
#include "CAnimManager.h"
#include "eAnimations.h"

using namespace plugin;

ThiscallEvent <AddressList<0x59FE09, H_CALL>, PRIORITY_BEFORE, ArgPickN<CObject*, 0>, void(CObject*)>  objectPreRenderEvent; // compatible with SilentPatch

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

		objectPreRenderEvent += [](CObject *obj)
		{
			if (obj->m_pObjectInfo->m_nSpecialColResponseCase == 6) // SWINGDOOR
			{
				obj->m_nPhysicalFlags.bCanBeCollidedWith = true; // record collisions

				for (int i = 0; i < 6; ++i)
				{
					if (obj->m_nNumEntitiesCollided > 0)
					{
						CEntity *entity = obj->m_apCollidedEntities[i];
						if (entity && entity->m_nType == ENTITY_TYPE_PED && DistanceBetweenPoints(entity->GetPosition(), obj->GetPosition()) < 3.0f)
						{
							CPed *ped = reinterpret_cast<CPed*>(entity);
							PedExtData &xdata = pedExtData.Get(ped);
							if ((CTimer::m_snTimeInMilliseconds - xdata.animDoorLastTime) > 1700 && ped->m_nMoveState > 1)
							{
								if (!ped->m_pIntelligence->GetTaskUseGun() &&
									!ped->m_pIntelligence->GetTaskHold(true) &&
									!ped->m_pIntelligence->GetTaskFighting() &&
									!ped->m_pIntelligence->GetTaskInAir() &&
									!ped->m_pIntelligence->GetTaskDuck(true) &&
									!ped->m_pIntelligence->GetTaskSwim() && // maybe some map mod is using doors underwater...
									!ped->m_pFire &&
									ped->m_fHealth > 0.0f)
								{
									CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_DEFAULT_WALK_DOORPARTIAL, 4.0);
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
