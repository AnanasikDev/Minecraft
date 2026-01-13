#include "Collider.h"

Collider::Collider(std::vector<AABB3D> boxes)
{
    m_boxes = boxes;
}

Collider Collider::FullBlock()
{
	return Collider(std::vector<AABB3D> 
	{ 
		AABB3D(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1))
	});
}

void Collider::AddBox(AABB3D aabb)
{
    m_boxes.push_back(aabb);
}

void Collider::AddBox(glm::vec3 min, glm::vec3 max)
{
    m_boxes.push_back(AABB3D(min, max));
}

Intersection Collider::GetIntersection(const Collider& other) const
{
    Intersection result;

    for (const auto& myBox : m_boxes)
    {
        for (const auto& otherBox : other.m_boxes)
        {
            if (myBox.Intersects(otherBox))
            {
                AABB3D overlap = AABB3D::GetIntersection(myBox, otherBox);

                result.m_exists = true;
                result.m_size = overlap.GetSize();

                return result;
            }
        }
    }

    return result;
}
