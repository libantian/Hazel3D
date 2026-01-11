#include "hzpch.h"
#include "OrthographicCameraController.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/Window.h"

namespace Hazel {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio),m_Camera(-m_AspectRatio*m_ZoomLevel,m_AspectRatio*m_ZoomLevel,-m_ZoomLevel,m_ZoomLevel),m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{

		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraPosition.x -= m_CameraTranslationSpeed * m_ZoomLevel * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraPosition.x += m_CameraTranslationSpeed * m_ZoomLevel * ts;;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
			m_CameraPosition.y += m_CameraTranslationSpeed * m_ZoomLevel * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
			m_CameraPosition.y -= m_CameraTranslationSpeed * m_ZoomLevel * ts;
		if (m_Rotation) 
		{
			if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}
		m_Camera.SetPosition(m_CameraPosition);
		
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= 0.1f * e.GetYOffset();
		ClampZoomLevel();
		m_Camera.SetProjection(
			-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel,
			-m_ZoomLevel, m_ZoomLevel
		);

		return true;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(
			-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel,
			-m_ZoomLevel, m_ZoomLevel
		);
		return true;
	}
}
