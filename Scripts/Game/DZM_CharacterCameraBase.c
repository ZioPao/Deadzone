modded class CharacterCameraBase : ScriptedCameraItem
{
	


	override float UpdateUDAngle(out float pAngle, float pMin, float pMax, float pDt)
	{
		pAngle = Math.Clamp(m_CharacterHeadAimingComponent.GetAimingRotation()[1], pMin, pMax);
		//On Foot
		if (m_CompartmentAccessComponent && !m_CompartmentAccessComponent.IsInCompartment())
			pAngle += m_ControllerComponent.GetAimingAngles()[1];
		
		return pAngle;
	}

}


modded class CharacterCamera1stPerson : CharacterCameraBase
{

	ref map<string, string> dzmSettings;
	const string DZM_FileNameJson = "DZM_Settings.json";
	const string DZM_MOD_ID = "5A0360779BCE26BE";		
	
	
	float yAxisAngle;
	float vel;
	
	
	float deadzoneStoppingPoint;
	bool lookingUpOrDown;
	
	
	void CharacterCamera1stPerson(CameraHandlerComponent pCameraHandler)
	{
		m_pCompartmentAccess = m_OwnerCharacter.GetCompartmentAccessComponent();
		// in head bone space
		m_OffsetLS = "0.0 0.03 -0.07";
		m_ApplyHeadBob = false;
		vel = 0.0;
		dzmSettings = new map<string, string>();
	}
	
	override void OnUpdate(float pDt, out ScriptedCameraItemResult pOutResult)
	{
		
		super.OnUpdate(pDt, pOutResult);
		////////////////////////////////////////////////////////////////////////////////
		OrderedVariablesMap dzmVariablesMap = new OrderedVariablesMap();
		
		dzmVariablesMap.Set("waitTimeBetweenFrames", new VariableInfo("Wait between frames", "0.033", EFilterType.TYPE_FLOAT));
		
		if (!MCF_SettingsManager.GetJsonManager(DZM_MOD_ID))
		{
			MCF_SettingsManager.Setup(DZM_MOD_ID, DZM_FileNameJson, dzmVariablesMap);
		
		}
		else if (!dzmSettings)
		{
			dzmSettings = MCF_SettingsManager.GetModSettings(DZM_MOD_ID);
			MCF_SettingsManager.GetJsonManager(DZM_MOD_ID).SetUserHelpers(dzmVariablesMap);		
		}


		pOutResult.m_fUseHeading = 0.0;
		
		float udAngle = UpdateUDAngle(m_fUpDownAngle, CONST_UD_MIN, CONST_UD_MAX, pDt);

		
		//! yaw pitch roll vector
		vector lookAngles;
		lookAngles[0] = m_fLeftRightAngle;
		lookAngles[2] = 0;
		yAxisAngle = Math.Clamp(udAngle + GetInterpolatedUDTransformAngle(pDt), CONST_UD_MIN, CONST_UD_MAX);
		
		float weaponAimingDir = m_ControllerComponent.GetWeaponAngles()[1];

		// 10 is the value we want to compensate for
		if (Math.AbsFloat(weaponAimingDir - deadzoneStoppingPoint) > 25)
		{
			lookAngles[1] = Math.SmoothCD(lookAngles[1], yAxisAngle, vel, 0.4, 1000, pDt);

			if (Math.AbsFloat(lookAngles[1]) - Math.AbsFloat(yAxisAngle) < 0.01)
			{
				deadzoneStoppingPoint = lookAngles[1];
				Print("Set new deadzone stopping point");
			
			}
		
		}
		//Print(weaponAimingDir);
	
		

		
		
		//! update fov
		m_fFOV = GetBaseFOV();
		if (m_bCameraTransition)
		{
			pOutResult.m_fUseHeading = 0.0;
			lookAngles[0] = lookAngles[0] + 180.0;
			pOutResult.m_iDirectBoneMode = EDirectBoneMode.RelativeDirection;
		}
		//! apply to rotation matrix
		Math3D.AnglesToMatrix(lookAngles, pOutResult.m_CameraTM);
		
		
		
		//CharacterCommandHandlerComponent m_CmdHandler = CharacterCommandHandlerComponent.Cast(m_OwnerCharacter.FindComponent(CharacterCommandHandlerComponent));

		//Print(m_CmdHandler);
		

		
	}


}


class DeadzoneScriptCharacterCommandMoveSettings : ScriptCharacterCommandMoveSettings 
{
	


}


