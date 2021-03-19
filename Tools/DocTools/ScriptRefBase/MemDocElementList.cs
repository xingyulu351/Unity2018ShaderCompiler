using System.Collections.Generic;
using System.Linq;

namespace ScriptRefBase
{
    public class MemDocElementList : IMemDocElement
    {
        public readonly List<IMemDocElement> m_Elements;
        public MemDocElementList()
        {
            m_Elements = new List<IMemDocElement>();
        }

        public MemDocElementList(IMemDocElement el)
        {
            m_Elements = new List<IMemDocElement> {el};
        }

        public MemDocElementList(IMemDocElement el, IMemDocElement el2)
        {
            m_Elements = new List<IMemDocElement> { el, el2 };
        }

        public MemDocElementList(IEnumerable<IMemDocElement> el)
        {
            m_Elements = el.ToList();
        }

        public bool IsDummyText()
        {
            if (m_Elements.Count != 1)
                return false;
            var onlyElement = m_Elements[0] as MemDocElementPlainText;
            return onlyElement != null && onlyElement.IsEmpty();
        }

        public void AddElements(MemDocElementList descriptionBefore, IMemDocElement resolvedPattern, MemDocElementList descriptionAfter)
        {
            if (!descriptionBefore.IsDummyText())
                m_Elements.AddRange(descriptionBefore.m_Elements);
            var patternAsList = resolvedPattern as MemDocElementList;
            if (patternAsList != null)
                m_Elements.AddRange(patternAsList.m_Elements);
            else
                m_Elements.Add(resolvedPattern);
            if (!descriptionAfter.IsDummyText())
                m_Elements.AddRange(descriptionAfter.m_Elements);
        }

        public string PlainText
        {
            get
            {
                return m_Elements.Aggregate("", (current, element) => current + element.PlainText);
            }
        }
    }
}
