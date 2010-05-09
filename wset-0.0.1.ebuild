# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils

DESCRIPTION="wset is a tool which allows you to compose wallpapers for X"
HOMEPAGE="http://github.com/niamster/wset"

LICENSE="GPL"
SLOT="0"
KEYWORDS="x86"
IUSE=""

SRC_URI="http://github.com/niamster/wset/tarball/master/${P}.tar.gz"

RDEPEND="media-libs/imlib2
    dev-python/pygtk
    x11-base/xorg-server"
DEPEND="${RDEPEND}"

src_unpack() {
   unpack "${A}"

   mv "${WORKDIR}/niamster-wset-685d5f9" "${WORKDIR}/${P}"
}

src_compile() {
    python setup.py build || die "Compilation failed"
}

src_install() {
    python setup.py install --prefix /usr/ --root "${D}" || die "Installation failed"
}
