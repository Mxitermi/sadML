#pragma once

#include "../dependencies/glm-1.0.3/glm/glm/glm.hpp"
#include "../dependencies/glm-1.0.3/glm/glm/ext/matrix_transform.hpp"
#include "../dependencies/glm-1.0.3/glm/glm/gtc/matrix_transform.hpp"

class Camera {
public:
	
	Camera(float fov, float width, float height) {
		projection = glm::perspective(fov / 2.0f, width / height, 0.1f, 1000.0f);
		view = glm::mat4(1.0f);
		update();

		position = glm::vec3(0.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.0f;
		pitch = 0.0f;
		onMouseMoved(0.0f, 0.0f);
		update();
	}

	void onMouseMoved(float xRel, float yRel) {
		yaw += xRel * mouseSensitivity;
		pitch -= yRel * mouseSensitivity;
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		
		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		lookAt = glm::normalize(front);
		update();
	}

	glm::mat4 getViewProj() {
		return viewProj;
	}

	void update() {
		view = glm::lookAt(position, position + lookAt, up);
		viewProj = projection * view;
	}

	void translate(glm::vec3 v) {
		position += v;
		view = glm::translate(view, v * -1.0f);
	}

	void moveFront(float amount) {
		translate(glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)*lookAt) * amount);
		update();
	}

	void moveSideways(float amount) {
		translate(glm::normalize(glm::cross(lookAt, up)) * -amount);
		update();
	}

	void moveUp(float amount) {
		translate(up * amount);
		update();
	}
	
private:
	glm::vec3 position;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 viewProj;
	float yaw;
	float pitch;
	float roll; // implementiren später auf Q und E
	glm::vec3 lookAt;
	const float mouseSensitivity = 0.3f;
	glm::vec3 up;
};