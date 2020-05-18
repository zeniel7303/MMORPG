using UnityEngine;
using System.Collections;

public class CameraMovement : MonoBehaviour {
	[Tooltip ("What the camera will look at."), SerializeField]
	private Transform target;
	[Tooltip ("How far the camera currently is from the target."), SerializeField]
	private float distance = 10.0f;
	[Tooltip ("How fast the camera moves horizontally."), SerializeField]
	private float xSpeed = 10.0f;
	[Tooltip ("How fast the camera moves vertically."), SerializeField]
	private float ySpeed = 10.0f;

	[Tooltip ("Minimum angle of the camera on the y axis."), SerializeField]
	private float yMinLimit = 10f;
	[Tooltip ("Maximum angle of the camera on the y axis."), SerializeField]
	private float yMaxLimit = 80f;

	[Tooltip ("Minimum angle of the camera on the x axis."), SerializeField]
	private float xMinLimit = -360f;
	[Tooltip ("Maximum angle of the camera on the x axis."), SerializeField]
	private float xMaxLimit = 360f;
   
	[Tooltip ("Minimum allowed distance between camera and target."), SerializeField]
	private float distanceMin = 0.5f;
	[Tooltip ("Maximum allowed distance between camera and target"), SerializeField]
	private float distanceMax = 10f;

	[Tooltip ("Radius of the thin SphereCast, used to detect camera collisions."), SerializeField]
	private float thinRadius = 0.15f;
	[Tooltip ("Radius of the thick SphereCast, used to detect camera collisions."), SerializeField]
	private float thickRadius = 0.3f;
	[Tooltip ("LayerMask used for detecting camera collisions. Camera will not avoid objects if this is not set correctly."), SerializeField]
	private LayerMask layerMask;

	private Quaternion rotation;
	private Vector3 position;
	private float x = 0.0f;
	private float y = 0.0f;

	void Start () {
		Vector3 angles = this.transform.eulerAngles;
		x = angles.y;
		y = angles.x;
	}

	void LateUpdate () {
		if (target) {
			CameraMove ();
			rotation = Quaternion.Euler (y, x, 0);

			if (distance < distanceMax) {
				distance = Mathf.Lerp (distance, distanceMax, Time.deltaTime * 2f);
			}

			Vector3 distanceVector = new Vector3 (0.0f, 0.0f, -distance);
			Vector3 position = rotation * distanceVector + target.position;
			transform.rotation = rotation;
			transform.position = position;
			CameraCollision ();
		}
	}

	public void CameraMove () {
		x += Input.GetAxis ("Mouse X") * xSpeed * 0.2f;
		y -= Input.GetAxis ("Mouse Y") * ySpeed * 0.2f;

		x = ClampAngle (x, xMinLimit, xMaxLimit);
		y = ClampAngle (y, yMinLimit, yMaxLimit);
	}

	public float ClampAngle (float angle, float min, float max) {
		if (angle < -360F) {
			angle += 360F;
		}

		if (angle > 360F) {
			angle -= 360F;
		}

		return Mathf.Clamp (angle, min, max);
	}

	void CameraCollision () {
		Vector3 normal, thickNormal;
		Vector3 ray = transform.position - target.position;

		Vector3 collisionPoint = GetDoubleSphereCastCollision (transform.position, thinRadius, out normal, true);
		Vector3 collisionPointThick = GetDoubleSphereCastCollision (transform.position, thickRadius, out thickNormal, false);
		Vector3 collisionPointRay = GetRayCollisionPoint (transform.position);

		Vector3 collisionPointProjectedOnRay = Vector3.Project (collisionPointThick - target.position, ray.normalized) + target.position;
		Vector3 vectorToProject = (collisionPointProjectedOnRay - collisionPointThick).normalized;
		Vector3 collisionPointThickProjectedOnThin = collisionPointProjectedOnRay - vectorToProject * thinRadius;
		float thinToThickDistance = Vector3.Distance (collisionPointThickProjectedOnThin, collisionPointThick);
		float thinToThickDistanceNormal = thinToThickDistance / (thickRadius - thinRadius);

		float collisionDistanceThin = Vector3.Distance (target.position, collisionPoint);
		float collisionDistanceThick = Vector3.Distance (target.position, collisionPointProjectedOnRay);

		float collisionDistance = Mathf.Lerp (collisionDistanceThick, collisionDistanceThin, thinToThickDistanceNormal);

		// Thick point can be actually projected IN FRONT of the character due to double projection to avoid sphere moving through the walls
		// In this case we should only use thin point
		bool isThickPointIncorrect = transform.InverseTransformDirection (collisionPointThick - target.position).z > 0;
		isThickPointIncorrect = isThickPointIncorrect || (collisionDistanceThin < collisionDistanceThick);
		if (isThickPointIncorrect) {
			collisionDistance = collisionDistanceThin;
		}

		if (collisionDistance < distance) {
			distance = collisionDistance;
		} else {
			distance = Mathf.SmoothStep (distance, collisionDistance, Time.deltaTime * 100 * Mathf.Max (distance * 0.1f, 0.1f));
		}

		distance = Mathf.Clamp (distance, distanceMin, distanceMax);
		transform.position = target.position + ray.normalized * distance;

		if (Vector3.Distance (target.position, collisionPoint) > Vector3.Distance (target.position, collisionPointRay)) {
			transform.position = collisionPointRay;
		} 
	}

	Vector3 GetDoubleSphereCastCollision (Vector3 cameraPosition, float radius, out Vector3 normal, bool pushAlongNormal) {
		float rayLength = 1;

		RaycastHit hit;
		Vector3 origin = target.position;
		Vector3 ray = origin - cameraPosition;
		float dot = Vector3.Dot (transform.forward, ray);

		if (dot < 0) {
			ray *= -1;
		}

		// Project the sphere in an opposite direction of the desired character->camera vector to get some space for the real spherecast
		if (Physics.SphereCast (origin, radius, ray.normalized, out hit, rayLength, layerMask)) {
			origin = origin + ray.normalized * hit.distance;
		} else {
			origin += ray.normalized * rayLength;
		}

		// Do final spherecast with offset origin
		ray = origin - cameraPosition;
		if (Physics.SphereCast (origin, radius, -ray.normalized, out hit, ray.magnitude, layerMask)) {
			normal = hit.normal;

			if (pushAlongNormal) {
				return hit.point + hit.normal * radius;
			} else {
				return hit.point;
			}
		} else {
			normal = Vector3.zero;
			return cameraPosition;
		}
	}

	Vector3 GetRayCollisionPoint (Vector3 cameraPosition) {
		Vector3 origin = target.position;
		Vector3 ray = cameraPosition - origin;

		RaycastHit hit;
		if (Physics.Raycast (origin, ray.normalized, out hit, ray.magnitude, layerMask)) {
			return hit.point + hit.normal * 0.15f;
		}

		return cameraPosition;
	}
}